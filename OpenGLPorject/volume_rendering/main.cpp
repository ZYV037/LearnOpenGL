#include <stdio.h>
#include <iostream>

#include "myshader.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "GL/freeglut.h"

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using glm::mat4;
using glm::vec3;

GLuint g_vao;
GLuint g_programHandle;
GLuint g_winWidth = 400;
GLuint g_winHeight = 400;
float g_angle = 0;
GLuint g_frameBuffer;
// transfer function
GLuint g_tffTexObj;
GLuint g_bfTexObj;
GLuint g_texWidth = 400;
GLuint g_texHeight = 400;
GLuint g_volTexObj;
GLuint g_rcVertHandle;
GLuint g_rcFragHandle;
GLuint g_bfVertHandle;
GLuint g_bfFragHandle;
float g_stepSize = 0.001f;

#define GL_ERROR() checkForOpenGLError(__FILE__, __LINE__)

int checkForOpenGLError(const char* file, int line)
{
    // return 1 if an OpenGL error occured, 0 otherwise.
    GLenum glErr;
    int retCode = 0;

    glErr = glGetError();
    while(glErr != GL_NO_ERROR)
    {
    cout << "glError in file " << file
         << "@line " << line << glErr << endl;
    retCode = 1;
    exit(EXIT_FAILURE);
    }
    return retCode;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int modes)
{
    switch(key)
    {
    case GLFW_KEY_ESCAPE:
        glfwSetWindowShouldClose(window, true);
        break;

    default:
        break;
    }
}

void display(void);
void initVBO();
void initShader();
void initFrameBuffer(GLuint, GLuint, GLuint);
GLuint initTFF1DTex(const char* filename);
GLuint initFace2DTex(GLuint texWidth, GLuint texHeight);
GLuint initVol3DTex(const char* filename, GLuint width, GLuint height, GLuint depth);
void render(GLenum cullFace);

void init()
{
    g_texWidth = g_winWidth;
    g_texHeight = g_winHeight;
    initVBO();
    initShader();
    g_tffTexObj = initTFF1DTex("tff.dat");
    g_bfTexObj = initFace2DTex(g_texWidth, g_texHeight);
    g_volTexObj = initVol3DTex("head256.raw", 256, 256, 225);
    GL_ERROR();
    initFrameBuffer(g_bfTexObj, g_texWidth, g_texHeight);
    GL_ERROR();
}

void initVBO()
{
    GLfloat vertices[24] = {
    0.0, 0.0, 0.0,
    0.0, 0.0, 1.0,
    0.0, 1.0, 0.0,
    0.0, 1.0, 1.0,
    1.0, 0.0, 0.0,
    1.0, 0.0, 1.0,
    1.0, 1.0, 0.0,
    1.0, 1.0, 1.0
    };
// draw the six faces of the boundbox by drawwing triangles
// draw it contra-clockwise
// front: 1 5 7 3
// back: 0 2 6 4
// left��0 1 3 2
// right:7 5 4 6
// up: 2 3 7 6
// down: 1 0 4 5
    GLuint indices[36] = {
    1,5,7,
    7,3,1,
    0,2,6,
    6,4,0,
    0,1,3,
    3,2,0,
    7,5,4,
    4,6,7,
    2,3,7,
    7,6,2,
    1,0,4,
    4,5,1
    };
    GLuint gbo[2];

    glGenBuffers(2, gbo);
    GLuint vertexdat = gbo[0];
    GLuint veridxdat = gbo[1];
    glBindBuffer(GL_ARRAY_BUFFER, vertexdat);
    glBufferData(GL_ARRAY_BUFFER, 24*sizeof(GLfloat), vertices, GL_STATIC_DRAW);
    // used in glDrawElement()
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, veridxdat);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36*sizeof(GLuint), indices, GL_STATIC_DRAW);

    GLuint vao;
    glGenVertexArrays(1, &vao);
    // vao like a closure binding 3 buffer object: verlocdat vercoldat and veridxdat
    glBindVertexArray(vao);
    glEnableVertexAttribArray(0); // for vertexloc
    glEnableVertexAttribArray(1); // for vertexcol

    // the vertex location is the same as the vertex color
    glBindBuffer(GL_ARRAY_BUFFER, vertexdat);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLfloat *)NULL);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLfloat *)NULL);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, veridxdat);
    // glBindVertexArray(0);
    g_vao = vao;
}
void drawBox(GLenum glFaces)
{
    glEnable(GL_CULL_FACE);
    glCullFace(glFaces);
    glBindVertexArray(g_vao);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)NULL);
    glDisable(GL_CULL_FACE);
}

// check the compilation result
GLboolean compileCheck(GLuint shader)
{
    GLint err;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &err);
    if (GL_FALSE == err)
    {
    GLint logLen;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);
    if (logLen > 0)
    {
        char* log = (char *)malloc(logLen);
        GLsizei written;
        glGetShaderInfoLog(shader, logLen, &written, log);
        cerr << "Shader log: " << log << endl;
        free(log);
    }
    }
    return err;
}

GLuint initShaderObj(const GLchar* srcfile, GLenum shaderType)
{
    ifstream inFile(srcfile, ifstream::in);
    // use assert?
    if (!inFile)
    {
    cerr << "Error openning file: " << srcfile << endl;
    exit(EXIT_FAILURE);
    }

    const int MAX_CNT = 10000;
    GLchar *shaderCode = (GLchar *) calloc(MAX_CNT, sizeof(GLchar));
    inFile.read(shaderCode, MAX_CNT);
    if (inFile.eof())
    {
    size_t bytecnt = inFile.gcount();
    *(shaderCode + bytecnt) = '\0';
    }
    else if(inFile.fail())
    {
    cout << srcfile << "read failed " << endl;
    }
    else
    {
    cout << srcfile << "is too large" << endl;
    }
    // create the shader Object
    GLuint shader = glCreateShader(shaderType);
    if (0 == shader)
    {
    cerr << "Error creating vertex shader." << endl;
    }
    // cout << shaderCode << endl;
    // cout << endl;
    const GLchar* codeArray[] = {shaderCode};
    glShaderSource(shader, 1, codeArray, NULL);
    free(shaderCode);

    // compile the shader
    glCompileShader(shader);
    if (GL_FALSE == compileCheck(shader))
    {
    cerr << "shader compilation failed" << endl;
    }
    return shader;
}

GLint checkShaderLinkStatus(GLuint pgmHandle)
{
    GLint status;
    glGetProgramiv(pgmHandle, GL_LINK_STATUS, &status);
    if (GL_FALSE == status)
    {
    GLint logLen;
    glGetProgramiv(pgmHandle, GL_INFO_LOG_LENGTH, &logLen);
    if (logLen > 0)
    {
        GLchar * log = (GLchar *)malloc(logLen);
        GLsizei written;
        glGetProgramInfoLog(pgmHandle, logLen, &written, log);
        cerr << "Program log: " << log << endl;
    }
    }
    return status;
}

// link shader program
GLuint createShaderPgm()
{
    // Create the shader program
    GLuint programHandle = glCreateProgram();
    if (0 == programHandle)
    {
    cerr << "Error create shader program" << endl;
    exit(EXIT_FAILURE);
    }
    return programHandle;
}


// init the 1 dimentional texture for transfer function
GLuint initTFF1DTex(const char* filename)
{
    // read in the user defined data of transfer function
    ifstream inFile(filename, ifstream::in);
        if (!inFile)
    {
    cerr << "Error openning file: " << filename << endl;
    exit(EXIT_FAILURE);
    }

    const int MAX_CNT = 10000;
    GLubyte *tff = (GLubyte *) calloc(MAX_CNT, sizeof(GLubyte));
    inFile.read(reinterpret_cast<char *>(tff), MAX_CNT);
    if (inFile.eof())
    {
    size_t bytecnt = inFile.gcount();
    *(tff + bytecnt) = '\0';
    cout << "bytecnt " << bytecnt << endl;
    }
    else if(inFile.fail())
    {
    cout << filename << "read failed " << endl;
    }
    else
    {
    cout << filename << "is too large" << endl;
    }
    GLuint tff1DTex;
    glGenTextures(1, &tff1DTex);
    glBindTexture(GL_TEXTURE_1D, tff1DTex);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA8, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, tff);
    free(tff);    
    return tff1DTex;
}

// init the 2D texture for render backface 'bf' stands for backface
GLuint initFace2DTex(GLuint bfTexWidth, GLuint bfTexHeight)
{
    GLuint backFace2DTex;
    glGenTextures(1, &backFace2DTex);
    glBindTexture(GL_TEXTURE_2D, backFace2DTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, bfTexWidth, bfTexHeight, 0, GL_RGBA, GL_FLOAT, NULL);
    return backFace2DTex;
}

// init 3D texture to store the volume data used fo ray casting
GLuint initVol3DTex(const char* filename, GLuint w, GLuint h, GLuint d)
{
    
    FILE *fp;
    size_t size = w * h * d;
    GLubyte *data = new GLubyte[size];			  // 8bit
    if (!(fp = fopen(filename, "rb")))
    {
        cout << "Error: opening .raw file failed" << endl;
        exit(EXIT_FAILURE);
    }
    else
    {
        cout << "OK: open .raw file successed" << endl;
    }
    if ( fread(data, sizeof(char), size, fp)!= size)
    {
        cout << "Error: read .raw file failed" << endl;
        exit(1);
    }
    else
    {
        cout << "OK: read .raw file successed" << endl;
    }
    fclose(fp);


    //memcpy(data1, data, sizeof(data1));


    glGenTextures(1, &g_volTexObj);
    // bind 3D texture target
    glBindTexture(GL_TEXTURE_3D, g_volTexObj);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
    // pixel transfer happens here from client to OpenGL server
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RED, w, h, d, 0, GL_RED, GL_UNSIGNED_BYTE, data);

    delete []data;
    cout << "volume texture created" << endl;
    return g_volTexObj;
}

void checkFramebufferStatus()
{
    GLenum complete = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (complete != GL_FRAMEBUFFER_COMPLETE)
    {
    cout << "framebuffer is not complete" << endl;
    exit(EXIT_FAILURE);
    }
}

void rcSetUinforms()
{
    // setting uniforms such as
    // ScreenSize
    // StepSize
    // TransferFunc
    // ExitPoints i.e. the backface, the backface hold the ExitPoints of ray casting
    // VolumeTex the texture that hold the volume data i.e. head256.raw
    GLint screenSizeLoc = glGetUniformLocation(g_programHandle, "ScreenSize");
    if (screenSizeLoc >= 0)
    {
        glUniform2f(screenSizeLoc, (float)g_winWidth, (float)g_winHeight);
    }
    else
    {
    cout << "ScreenSize"
         << "is not bind to the uniform"
         << endl;
    }
    GLint stepSizeLoc = glGetUniformLocation(g_programHandle, "StepSize");
    GL_ERROR();
    if (stepSizeLoc >= 0)
    {
    glUniform1f(stepSizeLoc, g_stepSize);
    }
    else
    {
    cout << "StepSize"
         << "is not bind to the uniform"
         << endl;
    }
    GL_ERROR();
    GLint transferFuncLoc = glGetUniformLocation(g_programHandle, "TransferFunc");
    if (transferFuncLoc >= 0)
    {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_1D, g_tffTexObj);
    glUniform1i(transferFuncLoc, 0);
    }
    else
    {
    cout << "TransferFunc"
         << "is not bind to the uniform"
         << endl;
    }
    GL_ERROR();
    GLint backFaceLoc = glGetUniformLocation(g_programHandle, "ExitPoints");
    if (backFaceLoc >= 0)
    {
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, g_bfTexObj);
    glUniform1i(backFaceLoc, 1);
    }
    else
    {
    cout << "ExitPoints"
         << "is not bind to the uniform"
         << endl;
    }
    GL_ERROR();
    GLint volumeLoc = glGetUniformLocation(g_programHandle, "VolumeTex");
    if (volumeLoc >= 0)
    {
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_3D, g_volTexObj);
    glUniform1i(volumeLoc, 2);
    }
    else
    {
    cout << "VolumeTex"
         << "is not bind to the uniform"
         << endl;
    }

}

// init the shader object and shader program
void initShader()
{
// vertex shader object for first pass
    g_bfVertHandle = initShaderObj("shader/backface.vert", GL_VERTEX_SHADER);
// fragment shader object for first pass
    g_bfFragHandle = initShaderObj("shader/backface.frag", GL_FRAGMENT_SHADER);
// vertex shader object for second pass
    g_rcVertHandle = initShaderObj("shader/raycasting.vert", GL_VERTEX_SHADER);
// fragment shader object for second pass
    g_rcFragHandle = initShaderObj("shader/raycasting.frag", GL_FRAGMENT_SHADER);
// create the shader program , use it in an appropriate time
    g_programHandle = createShaderPgm();
// ��������ɫ������������������(��ѡ)
}

// link the shader objects using the shader program
void linkShader(GLuint shaderPgm, GLuint newVertHandle, GLuint newFragHandle)
{
    const GLsizei maxCount = 2;
    GLsizei count;
    GLuint shaders[maxCount];
    glGetAttachedShaders(shaderPgm, maxCount, &count, shaders);
    // cout << "get VertHandle: " << shaders[0] << endl;
    // cout << "get FragHandle: " << shaders[1] << endl;
    GL_ERROR();
    for (int i = 0; i < count; i++) {
    glDetachShader(shaderPgm, shaders[i]);
    }
    // Bind index 0 to the shader input variable "VerPos"
    glBindAttribLocation(shaderPgm, 0, "VerPos");
    // Bind index 1 to the shader input variable "VerClr"
    glBindAttribLocation(shaderPgm, 1, "VerClr");
    GL_ERROR();
    glAttachShader(shaderPgm,newVertHandle);
    glAttachShader(shaderPgm,newFragHandle);
    GL_ERROR();
    glLinkProgram(shaderPgm);
    if (GL_FALSE == checkShaderLinkStatus(shaderPgm))
    {
    cerr << "Failed to relink shader program!" << endl;
    exit(EXIT_FAILURE);
    }
    GL_ERROR();
}


// both of the two pass use the "render() function"
// the first pass render the backface of the boundbox
// the second pass render the frontface of the boundbox
// together with the frontface, use the backface as a 2D texture in the second pass
// to calculate the entry point and the exit point of the ray in and out the box.
void render(GLenum cullFace)
{
    GL_ERROR();
    //glScale(4.0f, 4.0f, 4.0f);

    glClearColor(0.2f,0.2f,0.2f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //  transform the box
    glm::mat4 projection = glm::perspective(glm::radians(60.0f), (GLfloat)g_winWidth/g_winHeight, 0.1f, 400.f);
    glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 2.0f),
                     glm::vec3(0.0f, 0.0f, 0.0f),
                     glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 model = mat4(1.0f);
    model *= glm::rotate(glm::radians(g_angle), glm::vec3(0.0f, 1.0f, 0.0f));
    // to make the "head256.raw" i.e. the volume data stand up.
    model *= glm::rotate(glm::radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
    model *= glm::translate(glm::vec3(-0.5f, -0.5f, -0.5f));
    // notice the multiplication order: reverse order of transform
    glm::mat4 mvp = projection * view * model;
    GLuint mvpIdx = glGetUniformLocation(g_programHandle, "MVP");
    if (mvpIdx >= 0)
    {
        glUniformMatrix4fv(mvpIdx, 1, GL_FALSE, &mvp[0][0]);
    }
    else
    {
        cerr << "can't get the MVP" << endl;
    }
    GL_ERROR();
    drawBox(cullFace);
    GL_ERROR();
    // glutWireTeapot(0.5);
}
void rotateDisplay()
{
    g_angle += 1.0f;
    glutPostRedisplay();
}
void reshape(int w, int h)
{
    g_winWidth = w;
    g_winHeight = h;
    g_texWidth = w;
    g_texHeight = h;
}

// init the framebuffer, the only framebuffer used in this program
void initFrameBuffer(GLuint texObj, GLuint texWidth, GLuint texHeight)
{
    // create a depth buffer for our framebuffer
    GLuint depthBuffer;
    glGenRenderbuffers(1, &depthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, texWidth, texHeight);

    // attach the texture and the depth buffer to the framebuffer
    glGenFramebuffers(1, &g_frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, g_frameBuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texObj, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
    checkFramebufferStatus();
    glEnable(GL_DEPTH_TEST);
}


void framebuffer_size_callback(GLFWwindow* /*window*/, int width, int heitht)
{
    reshape(width, heitht);
    glViewport( 0, 0, g_winWidth, g_winHeight);
}

int main()
{
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    //glutInitWindowSize(20, 20);

    // init glfw
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    //create main window
    GLFWwindow* window = glfwCreateWindow(g_winWidth, g_winHeight, "Volume Rendering", nullptr, nullptr);
    if(nullptr == window)
    {
        std::cout << "glfwCreateWindow fail!" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // init glad
    if( !gladLoadGLLoader( (GLADloadproc)glfwGetProcAddress) )
    {
        std::cout << "gladLoadGLLoader fail!" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);

    int nrAttributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
    std::cout << "Maximum nr of vertex attributes supported: " << nrAttributes << std::endl;

    float vertices[] = {
         0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
         0.0f, 0.5f,  0.0f, 0.0f, 0.0f, 1.0f
    };

    //glutIdleFunc(&rotateDisplay);



    init();

    while(!glfwWindowShouldClose(window))
    {
        g_angle += 1.0f;
        glEnable(GL_DEPTH_TEST);
        // test the gl_error
        GL_ERROR();
        // render to texture
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, g_frameBuffer);

        linkShader(g_programHandle, g_bfVertHandle, g_bfFragHandle);
        glUseProgram(g_programHandle);
        // cull front face
        render(GL_FRONT);
        glUseProgram(0);
        GL_ERROR();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        linkShader(g_programHandle, g_rcVertHandle, g_rcFragHandle);
        GL_ERROR();
        glUseProgram(g_programHandle);
        rcSetUinforms();
        GL_ERROR();
        // glUseProgram(g_programHandle);
        // cull back face
        render(GL_BACK);
        // need or need not to resume the state of only one active texture unit?
        // glActiveTexture(GL_TEXTURE1);
        // glBindTexture(GL_TEXTURE_2D, 0);
        // glDisable(GL_TEXTURE_2D);
        // glActiveTexture(GL_TEXTURE2);
        // glBindTexture(GL_TEXTURE_3D, 0);
        // glDisable(GL_TEXTURE_3D);
        // glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_1D, 0);
        // glDisable(GL_TEXTURE_1D);
        // glActiveTexture(GL_TEXTURE0);
        glUseProgram(0);
        GL_ERROR();

        //rotateDisplay();

        // // for test the first pass
        // glBindFramebuffer(GL_READ_FRAMEBUFFER, g_frameBuffer);
        // checkFramebufferStatus();
        // glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        // glViewport(0, 0, g_winWidth, g_winHeight);
        // glClearColor(0.0, 0.0, 1.0, 1.0);
        // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // GL_ERROR();
        // glBlitFramebuffer(0, 0, g_winWidth, g_winHeight,0, 0,
        // 		      g_winWidth, g_winHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);
        // glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // GL_ERROR();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

//    glDeleteVertexArrays(1, &VAO);
//    glDeleteBuffers(1, &VBO);

    glfwTerminate();
    return 0;
}
