#version 330 core

out vec2 TxStart;
out vec2 TxEnd;
out vec2 TxCentroid;
out ivec4 TxTriangle;

uniform float selected;
uniform float specExp;
uniform vec3 specCol;
uniform float opacity;
uniform vec3 ambCol;

in vec4 tCentroid;
in vec3 start;
in vec3 end;
flat in int triangleIndex;


void main()
{
    TxStart = vec2(start.x,start.y);
    TxEnd = vec2(end.x,end.y);
    TxCentroid = vec2(tCentroid.x,tCentroid.y);
    TxTriangle.r = ((triangleIndex>>0) %256) - 128;
    TxTriangle.g = ((triangleIndex>>8 )%256) - 128;
    TxTriangle.b = ((triangleIndex>>16)%256) - 128;
    TxTriangle.a = ((triangleIndex>>24)%256) - 128;
}