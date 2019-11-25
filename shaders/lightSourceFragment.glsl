#version 450 core

in vec3 loPosition;

out vec4 FragColor;

void main()
{
    FragColor = vec4(loPosition, 1.0f);
}