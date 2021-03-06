#version 450 core
layout(triangles) in;
layout(triangle_strip, max_vertices = 9) out;

in vec4[3] v_fragColour;
in vec3[3] v_vNorm;
in vec2[3] v_texCoord;
in vec3[3] v_worldVPos;
in vec4[3] v_FragPosLightSpace;

out vec4 fragColour;
out vec3 vNorm;
out vec2 texCoord;
out vec3 worldVPos;
out vec4 FragPosLightSpace;

out vec3 tCentroid;
out vec3 tNormal;
out vec3 edgeAlong;
out vec3 edgePerpendicular;

out int triangleIndex;

int shift(int toShift){
    return (toShift<<7)|(toShift>>(32-7));
}

void doVertex(int index){
    gl_Position = gl_in[index].gl_Position;
    fragColour = v_fragColour[index];
    vNorm = v_vNorm[index];
    texCoord = v_texCoord[index];
    worldVPos = v_worldVPos[index];
    FragPosLightSpace = v_FragPosLightSpace[index];
    EmitVertex();
}

void doCentroid(vec4 tCentroid){
    //fragColour = vec4(0,0,0,0);
    gl_Position = tCentroid;
    EmitVertex();
}

void setEdgeDir(vec4 posFrom, vec4 posTo,vec3 tNormal){
    edgeAlong = normalize(posTo.xyz - posFrom.xyz);
    edgePerpendicular = normalize(cross(tNormal, edgeAlong));
}

void main()
{
    vec4 posA = gl_in[0].gl_Position;
    vec4 posB = gl_in[1].gl_Position;
    vec4 posC = gl_in[2].gl_Position;
    vec3 WposA = v_worldVPos[0];
    vec3 WposB = v_worldVPos[1];
    vec3 WposC = v_worldVPos[2];

    int temp = 7*11*13*17;
    temp = (      temp  ^ floatBitsToInt(WposA.x)) % 2123000501;
    temp = (shift(temp) ^ floatBitsToInt(WposA.y)) % 2123000557;
    temp = (shift(temp) ^ floatBitsToInt(WposA.z)) % 2123000563;
    temp = (shift(temp) ^ floatBitsToInt(WposB.x)) % 2123000591;
    temp = (shift(temp) ^ floatBitsToInt(WposB.y)) % 2123000597;
    temp = (shift(temp) ^ floatBitsToInt(WposB.z)) % 2123000603;
    temp = (shift(temp) ^ floatBitsToInt(WposC.x)) % 2123000639;
    temp = (shift(temp) ^ floatBitsToInt(WposC.y)) % 2123000729;
    temp = (shift(temp) ^ floatBitsToInt(WposC.z)) % 2123000753;
    triangleIndex = temp;

    tCentroid = ((1.0/3.0) * (posA + posB + posC)).xyz;
    tNormal = cross((posC -posB).xyz, (posA- posB).xyz);

    setEdgeDir(posA, posB,tNormal);
    doVertex(0);
    doVertex(1);
    doVertex(2);
    EndPrimitive();
}