#version 460 core

out vec4 FragColor;

in vec3 ourColor;
in vec2 texCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;

uniform float rate;

void main()
{
    //FragColor = texture(ourTexture, texCoord)*vec4(ourColor, 1.0f);
    FragColor = mix(texture(texture1, texCoord), texture(texture2, vec2(1.0 - texCoord.x, texCoord.y)), rate);
}
