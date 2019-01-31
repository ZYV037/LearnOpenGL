﻿#include <iostream>
#include <glad/glad.h>
#include <glfw/glfw3.h>

void process_input(GLFWwindow* window)
{
    if( GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE) )
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

void frabe_buffersize_callback_fun(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}


char* vertexShaderSource = \
"#version 460 core\n    \
layout (location = 0) in vec3 aPos;\n   \
void main()\n \
{\n   \
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n    \
}\n\0";

char* fragmentShaderSource = \
"#version 460 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\n\0";


int main()
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(800, 600, "Hello Triangle", nullptr, nullptr);
    if(window == nullptr)
    {
        std::cout << "Failed to glfwCreateWindow()!" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if( !gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize glad!" << std::endl;
        return -1;
    }

    glfwSetFramebufferSizeCallback(window, frabe_buffersize_callback_fun);

    //create shader
    unsigned int vertex_shader;
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(vertex_shader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertex_shader);

    int sucess;
    char infoLog[512];
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &sucess);
    if(!sucess)
    {
        glGetShaderInfoLog(vertex_shader, 512, nullptr, infoLog);
        std::cout << "Error::SHADER::VERTEX::COMPILATION\n" << infoLog << std::endl;
    }

    unsigned int fragment_shader;
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(fragment_shader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragment_shader);

    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &sucess);
    if(!sucess)
    {
        glGetShaderInfoLog(vertex_shader, 512, nullptr, infoLog);
        std::cout << "Error::SHADER::FRAGMENT::COMPILATION\n" << infoLog << std::endl;
    }

    //programe link shaders
    unsigned int shader_programe;
    shader_programe = glCreateProgram();

    glAttachShader(shader_programe, vertex_shader);
    glAttachShader(shader_programe, fragment_shader);
    glLinkProgram(shader_programe);

    glGetProgramiv(shader_programe, GL_LINK_STATUS, &sucess);
    if(!sucess)
    {
        glGetProgramInfoLog(shader_programe, 512, nullptr, infoLog);
        std::cout << "Error::PROGRAME::LINK\n" << infoLog << std::endl;
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    float vertices[] =
    {
         0.5f, 0.5f,  0.0f,
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };

    float vertices2[] =
    {
        0.5f, 0.5f, 0.0f,   // 右上角
        0.5f, -0.5f, 0.0f,  // 右下角
        -0.5f, -0.5f, 0.0f, // 左下角
        -0.5f, 0.5f, 0.0f   // 左上角
    };

    unsigned int indices[] =
    { // 注意索引从0开始!
        0, 1, 3, // 第一个三角形
        1, 2, 3  // 第二个三角形
    };

    unsigned int EBO;
    glGenBuffers(1, &EBO);

    unsigned int VBO;
    glGenBuffers(1, &VBO);

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);

    glBindBuffer( GL_ARRAY_BUFFER, VBO);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), &vertices2, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0 );
    glEnableVertexAttribArray(0);

    //unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    while(!glfwWindowShouldClose(window))
    {
        process_input(window);

        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shader_programe);
        glBindVertexArray(VAO);
        //glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glDeleteProgram(shader_programe);

    glfwTerminate();
    return 0;
}
