#ifndef MYSHADER_H
#define MYSHADER_H

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class MyShader
{
public:
    MyShader(const GLchar* vertexPath, const GLchar* fragmentPath);

    void use();

    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;

private:
    std::string getFileContent(const GLchar* filePath);
    bool createShader(unsigned int& shader, const std::string shaderCode, GLuint shaderType);
    bool createPrograme(unsigned int vertex, unsigned int fragment);

    unsigned int ID;
};

#endif // MYSHADER_H
