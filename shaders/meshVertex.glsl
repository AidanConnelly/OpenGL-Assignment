#version 450 core
layout(location=0) in vec3 vPosition;
layout(location=1) in vec3 vNormal;
layout(location=2) in vec3 vColour;
layout(location=3) in vec2 aTexCoord;
uniform mat4 m;
uniform mat4 vp;

out vec4 fragColour;
out vec3 vNorm;
out vec2 texCoord;
out vec3 worldVPos;
void main()
{
	fragColour = vec4(vColour,1.0);
	gl_Position = vp * m * vec4 (vPosition,1.0);
	texCoord = aTexCoord;
	vNorm = vNormal;
	worldVPos =  vec3(m * vec4 (vPosition,1.0));
} 