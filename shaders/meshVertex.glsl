#version 450 core
layout(location=0) in vec3 vPosition;
layout(location=1) in vec3 vNormal;
layout(location=2) in vec3 vColour;
layout(location=3) in vec2 aTexCoord;
uniform mat4 m;
uniform mat4 v;
uniform mat4 p;

out vec4 v_fragColour;
out vec3 v_vNorm;
out vec2 v_texCoord;
out vec3 v_worldVPos;

void main()
{
	gl_Position = p * v * m * vec4 (vPosition,1.0);
	v_fragColour = vec4(vColour,1.0);
	v_texCoord = aTexCoord;
	v_vNorm = vNormal;
	v_worldVPos =  vec3(m * vec4 (vPosition,1.0));
} 