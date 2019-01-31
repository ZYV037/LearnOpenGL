#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <string>
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void precess_input(GLFWwindow* window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) ==  GLFW_PRESS )
    {
        glfwSetWindowShouldClose(window, true);
    }
}

int main(int argc, char *argv[])
{
//    typedef void (*GL_GENBUFFERS) (GLsizei, GLuint*);

//    GL_GENBUFFERS glGenBuffers = (GL_GENBUFFERS)wglGetProcAddress("glGenBuffers");
//    GLuint buffer;
//    glGenBuffers(1, &buffer);

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Learning OpenGL", nullptr, nullptr);
    if(window == nullptr)
    {
        std::cout << "Faild to craete GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Faild to initialize GLAD" << std::endl;
        return -1;
    }

    glfwMakeContextCurrent(window);
    //glViewport(0, 0, 800, 600);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    while( !glfwWindowShouldClose(window))
    {
        precess_input(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}
