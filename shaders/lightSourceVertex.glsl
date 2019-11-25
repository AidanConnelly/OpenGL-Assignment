#version 450 core

layout(location=0) in vec3 lPosition;

uniform mat4 vp;

out vec3 loPosition;

void main()
{
    loPosition = lPosition;
    gl_Position = vp * vec4 (lPosition,1.0);
//    gl_Position = vp * vec4 (0.0,0.0,0.0,1.0);
}