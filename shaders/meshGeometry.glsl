#version 450 core
layout(triangles) in;
layout(triangle_strip, max_vertices = 9) out;

in vec4[3] v_fragColour;
in vec3[3] v_vNorm;
in vec2[3] v_texCoord;
in vec3[3] v_worldVPos;

out vec4 fragColour;
out vec3 vNorm;
out vec2 texCoord;
out vec3 worldVPos;

out vec4 tCentroid;
out vec3 tNormal;
out vec3 edgeAlong;
out vec3 edgePerpendicular;

void doVertex(int index){
    gl_Position = gl_in[index].gl_Position;
    fragColour = v_fragColour[index];
    vNorm = v_vNorm[index];
    texCoord = v_texCoord[index];
    worldVPos = v_worldVPos[index];
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

    tCentroid = (1.0/3.0) * (posA + posB + posC);
    tNormal = cross((posC -posB).xyz, (posA- posB).xyz);

    setEdgeDir(posA, posB,tNormal);
    doVertex(0);
    doVertex(1);
//    doCentroid(tCentroid);
//    EndPrimitive();
//
//    setEdgeDir(posB, posC,tNormal);
//    doVertex(1);
//    doVertex(2);
//    doCentroid(tCentroid);
//    EndPrimitive();
//
//    setEdgeDir(posC, posA,tNormal);
    doVertex(2);
//    doVertex(0);
//    doCentroid(tCentroid);
    EndPrimitive();
}