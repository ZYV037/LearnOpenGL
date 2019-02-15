#include "myshader.h"

MyShader::MyShader(const GLchar* vertexPath, const GLchar* fragmentPath)
{
    std::string vertexShaderCode = getFileContent(vertexPath);
    std::string fragmentShaderCode = getFileContent(fragmentPath);

    unsigned int vertex, fragment;
    createShader(vertex, vertexShaderCode, GL_VERTEX_SHADER);
    createShader(fragment, fragmentShaderCode, GL_FRAGMENT_SHADER);

    createPrograme(vertex, fragment);

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void MyShader::use()
{
    glUseProgram(ID);
}

void MyShader::setBool(const std::string &name, bool value) const
{
    glUniform1i(glad_glGetUniformLocation(ID, name.c_str()), (int)value );
}

void MyShader::setInt(const std::string& name, int value) const
{
    glUniform1i(glad_glGetUniformLocation(ID, name.c_str()), (int)value);
}

void MyShader::setFloat(const std::string& name, float value) const
{
    glUniform1f(glad_glGetUniformLocation(ID, name.c_str()), value);
}

void MyShader::setMat4(const std::string& name, glm::mat4& value) const
{
    glUniformMatrix4fv(glad_glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &value[0][0]);
}

std::string MyShader::getFileContent(const GLchar* filePath)
{
    std::string shaderCode;
    std::ifstream shaderFile;
    shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        shaderFile.open(filePath);
        std::stringstream shaderStream;
        shaderStream << shaderFile.rdbuf();
        shaderFile.close();
        shaderCode = shaderStream.str();
    }
    catch (std::ifstream::failure e)
    {
        std::cout << "Error: Shader::Read shader file"  << filePath <<  "Failed!" << std::endl;
    }

    return shaderCode;
}

bool MyShader::createShader(unsigned int& shader, const std::string shaderCode, GLuint shaderType)
{
    int success;
    char infoLog[512];

    shader = glCreateShader(shaderType);
    const char* vertexCode = shaderCode.c_str();
    glShaderSource(shader, 1, &vertexCode, nullptr);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cout << "Error::Shader::compile failed!\n" << infoLog << std::endl;
        return false;
    }

    return true;
}

bool MyShader::createPrograme(unsigned int vertex, unsigned int fragment)
{
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);

    glLinkProgram(ID);
    int success;
    char infoLog[512];
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(ID, 512, nullptr, infoLog);
        std::cout << "Error::Programe::link failed!\n" << infoLog << std::endl;
        return false;
    }

    return true;
}
