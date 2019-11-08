#version 450 core
layout(location=0) in vec3 vPosition;
layout(location=1) in vec3 vNormal;
layout(location=2) in vec3 vColour;
layout(location=3) in vec2 aTexCoord;
uniform mat4 mvp;

out vec4 fragColour;
out vec2 texCoord;
void main()
{
	fragColour = vec4(1.0,1.0,1.0,1.0);
	gl_Position = mvp * vec4 (vPosition,1.0);
	texCoord = aTexCoord;
} 