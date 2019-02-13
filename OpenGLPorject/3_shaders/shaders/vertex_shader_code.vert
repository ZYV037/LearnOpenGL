#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 ourColor;

uniform float transX;

void main()
{
    gl_Position = vec4(aPos, 1.0f);
    //gl_Position = vec4(aPos.x + transX, -aPos.y, aPos.z, 1.0f);
    //ourColor = aColor;
    ourColor = gl_Position.xyz;
}
