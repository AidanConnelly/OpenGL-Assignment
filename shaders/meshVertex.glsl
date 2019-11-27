#version 450 core
layout(location=0) in vec3 vPosition;
layout(location=1) in vec3 vNormal;
layout(location=2) in vec3 vColour;
layout(location=3) in vec2 aTexCoord;
uniform mat4 m;
uniform mat4 v;
uniform mat4 p;

out vec4 fragColour;
out vec3 vNorm;
out vec2 texCoord;
out vec3 worldVPos;

void main()
{
	fragColour = vec4(vColour,1.0);
	vec4 viewPos = v * m * vec4 (vPosition,1.0);
	float z = viewPos.z;
	float hyPot = sqrt(viewPos.x*viewPos.x + viewPos.y * viewPos.y);
	float theta = atan(hyPot,z);
	float phi = atan(viewPos.y,viewPos.x);
	float r = sqrt(hyPot*hyPot+viewPos.z*viewPos.z);

	vec3 dispPos = vec3(0,0,0);
	float dispR = theta / 3.1415926538;
	dispPos.x = dispR * cos(phi);
	dispPos.y = dispR * sin(phi);

	float near = 0.01;
	float far = 100.01;
	dispPos.z = (r-near)/(far-near);

	gl_Position = vec4(dispPos,1.0);
	texCoord = aTexCoord;
	vNorm = vNormal;
	worldVPos =  vec3(m * vec4 (vPosition,1.0));
} 