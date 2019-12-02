#version 450 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 9) out;

out vec4 tCentroid;
out vec3 tNormal;
out vec3 start;
out vec3 end;

out vec3 vPos;
out int triangleIndex;

in vec3 wPos[3];



uniform mat4 lightSpaceMatrix;

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

int shift(int toShift){
    return (toShift<<7)|(toShift>>(32-7));
}

void main(){
    vec4 posA = gl_in[0].gl_Position;
    vec4 posB = gl_in[1].gl_Position;
    vec4 posC = gl_in[2].gl_Position;

    int temp = 7*11*13*17;
    temp = (      temp  ^ floatBitsToInt(wPos[0].x)) % 2123000501;
    temp = (shift(temp) ^ floatBitsToInt(wPos[0].y)) % 2123000557;
    temp = (shift(temp) ^ floatBitsToInt(wPos[0].z)) % 2123000563;
    temp = (shift(temp) ^ floatBitsToInt(wPos[1].x)) % 2123000591;
    temp = (shift(temp) ^ floatBitsToInt(wPos[1].y)) % 2123000597;
    temp = (shift(temp) ^ floatBitsToInt(wPos[1].z)) % 2123000603;
    temp = (shift(temp) ^ floatBitsToInt(wPos[2].x)) % 2123000639;
    temp = (shift(temp) ^ floatBitsToInt(wPos[2].y)) % 2123000729;
    temp = (shift(temp) ^ floatBitsToInt(wPos[2].z)) % 2123000753;
    triangleIndex = temp;

    tCentroid = (1.0/3.0) * (posA + posB + posC);
    tNormal = cross((posC -posB).xyz, (posA- posB).xyz);

    start = (posA.xyz);
    end = (posB.xyz);
    doVertex(0);
    doVertex(1);
    doCentroid(tCentroid);
    EndPrimitive();

    start = (posB.xyz);
    end = (posC.xyz);
    doVertex(1);
    doVertex(2);
    doCentroid(tCentroid);
    EndPrimitive();

    start = (posC.xyz);
    end = (posA.xyz);
    doVertex(2);
    doVertex(0);
    doCentroid(tCentroid);
    EndPrimitive();
}