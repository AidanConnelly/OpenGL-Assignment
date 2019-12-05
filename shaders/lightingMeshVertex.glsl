#version 450 core
layout(location=0) in vec3 vPosition;
layout(location=1) in vec3 vNormal;
layout(location=2) in vec3 vColour;
layout(location=3) in vec2 aTexCoord;
uniform mat4 m;
uniform mat4 v;
uniform mat4 p;
uniform mat4 lightSpaceMatrix;

out vec4 fragColour;
out vec3 vNorm;
out vec2 texCoord;
out vec3 worldVPos;
out vec4 FragPosLightSpace;

void main()
{
	gl_Position = p * v * m * vec4(vPosition,1.0);
	fragColour = vec4(vColour,1.0);
	texCoord = aTexCoord;
	vNorm = vNormal;
	worldVPos = vec3(m * vec4 (vPosition,1.0));
	FragPosLightSpace = lightSpaceMatrix * vec4(worldVPos,1.0);
} 