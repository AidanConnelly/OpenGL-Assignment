#version 330 core

out vec2 TxStart;
out vec2 TxEnd;
out vec2 TxCentroid;
out float TxTriangle;

uniform float selected;
uniform float specExp;
uniform vec3 specCol;
uniform float opacity;
uniform vec3 ambCol;

in vec4 tCentroid;
in vec3 edgeAlong;
in vec3 start;
flat in float triangleIndex;


void main()
{
    vec3 end = start + edgeAlong;
    TxStart = vec2(-start.x/start.z,-start.y/start.z);
    TxEnd = vec2(-end.x/end.z,-end.y/end.z);
    TxCentroid = vec2(-tCentroid.x/tCentroid.z,-tCentroid.y/tCentroid.z);
    TxTriangle = triangleIndex;

}