#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 lightSpaceMatrix;
uniform mat4 m;

out vec3 wPos;

void main()
{
    vec4 wPos4 = m * vec4(aPos,1.0);
    wPos = vec3(wPos4);
    gl_Position = lightSpaceMatrix * wPos4;
}