#version 330 core

in vec3 FragPos;

out vec4 color;

uniform vec4 outputColor;

void main()
{    
    color = outputColor;
}