#include <stdio.h>
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

void process_input(GLFWwindow* window)
{
    if( GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE))
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

void frame_buffersize_callback_fun(GLFWwindow* window, int width, int height)
{
    glViewport( 0, 0, width, height);
}

int main()
{
    // init glfw
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    //create main window
    GLFWwindow* window = glfwCreateWindow(800, 600, "Shaders", nullptr, nullptr);
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

    glfwSetFramebufferSizeCallback(window, frame_buffersize_callback_fun);

    int nrAttributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
    std::cout << "Maximum nr of vertex attributes supproted: " << nrAttributes << std::endl;

    return 0;
}
