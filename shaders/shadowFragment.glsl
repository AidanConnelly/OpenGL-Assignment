#version 330 core

out vec2 TxStart;
out vec2 TxEnd;

uniform float selected;
uniform float specExp;
uniform vec3 specCol;
uniform float opacity;
uniform vec3 ambCol;

in vec3 edgeAlong;
in vec3 start;


void main()
{
    vec3 end = start + edgeAlong;
    TxStart = vec2(start.x/start.z,start.y/start.z);
    TxEnd = vec2(end.x/end.z,end.y/end.z);
}