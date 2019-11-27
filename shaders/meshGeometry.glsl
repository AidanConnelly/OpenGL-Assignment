#version 450 core
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

out vec3 posA;
out vec3 posB;
out vec3 posC;

in vec4[3] v_fragColour;
in vec3[3] v_vNorm;
in vec2[3] v_texCoord;
in vec3[3] v_worldVPos;

out vec4 fragColour;
out vec3 vNorm;
out vec2 texCoord;
out vec3 worldVPos;

void doVertex(int i){
    gl_Position = gl_in[i].gl_Position;
    fragColour = v_fragColour[i];
    vNorm = v_vNorm[i];
    texCoord = v_texCoord[i];
    worldVPos = v_worldVPos[i];
    EmitVertex();
}

void main()
{
    posA = gl_in[0].gl_Position.xyz;
    posB = gl_in[1].gl_Position.xyz;
    posC = gl_in[2].gl_Position.xyz;

    doVertex(0);
    doVertex(1);
    doVertex(2);
    EndPrimitive();
}