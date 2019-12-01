#version 330 core

out ivec2 TxStart;
out ivec2 TxEnd;
out ivec2 TxCentroid;
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

#define MAX_UINT16 32767.0
#define MARGIN 0.5

int toInt(float toConvert){
    float addedMargin = toConvert /(1.0 + MARGIN);
    float encoded = MAX_UINT16* addedMargin;
    return int(encoded);
}

ivec2 toIntVec(vec2 toConvert){
    return ivec2(toInt(toConvert.x), toInt(toConvert.y));
}

void main()
{
    TxStart = toIntVec(vec2(-start.x,-start.y));
    TxEnd = toIntVec(vec2(-end.x,-end.y));
    TxCentroid = toIntVec(vec2(-tCentroid.x,-tCentroid.y));
    TxTriangle.r = ((triangleIndex>>0) %256) - 128;
    TxTriangle.g = ((triangleIndex>>8 )%256) - 128;
    TxTriangle.b = ((triangleIndex>>16)%256) - 128;
    TxTriangle.a = ((triangleIndex>>24)%256) - 128;
}