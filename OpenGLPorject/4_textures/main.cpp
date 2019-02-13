#include <iostream>

#include "myshader.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "stb_image.h"

void FrameBuffersizeCallBackFunction(GLFWwindow* /*window*/, int width, int height)
{
    glViewport(0, 0, width, height);
}

void ProcessInput(GLFWwindow* window, MyShader& shader)
{
    static float rate = 0.5f;
    if( GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE))
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    if(GLFW_PRESS == glfwGetKey(window, GLFW_KEY_DOWN))
    {
        rate -= 0.05;
        shader.setFloat("rate", rate);
    }
    else if(GLFW_PRESS == glfwGetKey(window, GLFW_KEY_UP))
    {
        rate += 0.05;
        shader.setFloat("rate", rate);
    }

}

int main(int /*argc*/, char */*argv*/[])
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_COMPAT_PROFILE, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(800, 600, "Textures", nullptr, nullptr);
    if(nullptr == window)
    {
        std::cout << "glfwCreateWindow fail!" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if( !gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "gladLoadGLLoader fail!" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwSetFramebufferSizeCallback(window, FrameBuffersizeCallBackFunction);

    float vertices[] = {
        0.5, 0.5, 0.0, 1.0f, 0.0f, 0.0f, 0.55f, 0.55f,
        0.5f, -0.5, 0.0, 0.0f, 1.0f, 0.0f, 0.55f, 0.45f,
        -0.5f, -0.5f, 0.0, 1.0f, 1.0f, 0.0f, 0.45f, 0.45f,
        -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.45f, 0.55f
    };

    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3
    };

    unsigned int VAO;
    glGenVertexArrays( 1, &VAO );

    unsigned int VBO;
    glGenBuffers(1, &VBO);

    unsigned int EBO;
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, FALSE, 8*sizeof(float), (void*)(0) );
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, FALSE, 8*sizeof(float), (void*)(3*sizeof(float)) );
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, FALSE, 8*sizeof(float), (void*)(6*sizeof(float)) );
    glEnableVertexAttribArray(2);

    int width, height, nrChannels;
    unsigned char* data = stbi_load("../4_textures/container.jpg", &width, &height, &nrChannels, 0);
    if(data == nullptr)
    {
        std::cout << "Error: stbi_load ../4_textures/container.jpg failed!" << std::endl;
    }

    unsigned int texture1;
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    //float borderColor[] = {1.0f, 1.0f, 0.0f, 1.0f };
    //glTexParameterfv( GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    stbi_set_flip_vertically_on_load(true);
    data = stbi_load("../4_textures/awesomeface.png", &width, &height, &nrChannels, 0);
    if(data == nullptr)
    {
        std::cout << "Error: stbi_load ../4_textures/awesomeface.png failed!" << std::endl;
    }

    unsigned int texture2;
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    //float borderColor[] = {1.0f, 1.0f, 0.0f, 1.0f };
    //glTexParameterfv( GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    MyShader shaderPrograme("../4_textures/vertex_shader_code.vert", "../4_textures/fragment_shader_code.frag");
    shaderPrograme.use();
    shaderPrograme.setInt("texture1", 0);
    shaderPrograme.setInt("texture2", 1);

    shaderPrograme.setFloat("rate", 0.5);

    while( !glfwWindowShouldClose(window))
    {
        ProcessInput(window, shaderPrograme);

        glClearColor(0.2, 0.3, 0.4, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);
        //glBindVertexArray(VAO);
        //glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();
    return 0;
}
