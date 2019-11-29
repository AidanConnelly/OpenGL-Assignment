#version 450 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 9) out;

out vec4 tCentroid;
out vec3 tNormal;
out vec3 edgeAlong;
out vec3 start;

out vec3 vPos;
out int triangleIndex;

in vec3 wPos[3];

void doVertex(int index){
    gl_Position = gl_in[index].gl_Position;
    vPos = vec3(gl_Position);
    EmitVertex();
}

void doCentroid(vec4 tCentroid){
    gl_Position = tCentroid;
    vPos = vec3(gl_Position);
    EmitVertex();
}

void setEdgeDir(vec4 posFrom, vec4 posTo,vec3 tNormal){
    edgeAlong = posTo.xyz - posFrom.xyz;
}

int shift(int toShift){
    return (toShift<<7)|(toShift>>(32-7));
}

void main(){
    vec4 posA = gl_in[0].gl_Position;
    vec4 posB = gl_in[1].gl_Position;
    vec4 posC = gl_in[2].gl_Position;

    int temp = 7*11*13*17;
    temp =       temp  ^ floatBitsToInt((wPos[0]).x);
    temp = shift(temp) ^ floatBitsToInt((wPos[0]).y);
    temp = shift(temp) ^ floatBitsToInt((wPos[0]).z);
    temp = shift(temp) ^ floatBitsToInt((wPos[1]).x);
    temp = shift(temp) ^ floatBitsToInt((wPos[1]).y);
    temp = shift(temp) ^ floatBitsToInt((wPos[1]).z);
    temp = shift(temp) ^ floatBitsToInt((wPos[2]).x);
    temp = shift(temp) ^ floatBitsToInt((wPos[2]).y);
    temp = shift(temp) ^ floatBitsToInt((wPos[2]).z);
    triangleIndex = temp;

    tCentroid = (1.0/3.0) * (posA + posB + posC);
    tNormal = cross((posC -posB).xyz, (posA- posB).xyz);

    start = posA.xyz;
    setEdgeDir(posA, posB,tNormal);
    doVertex(0);
    doVertex(1);
    doCentroid(tCentroid);
    EndPrimitive();

    start = posB.xyz;
    setEdgeDir(posB, posC,tNormal);
    doVertex(1);
    doVertex(2);
    doCentroid(tCentroid);
    EndPrimitive();

    start = posC.xyz;
    setEdgeDir(posC, posA,tNormal);
    doVertex(2);
    doVertex(0);
    doCentroid(tCentroid);
    EndPrimitive();
}