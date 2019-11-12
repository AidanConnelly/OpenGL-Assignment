#version 450 core

out vec4 fColor;
in vec4 fragColour;
in vec2 texCoord;

uniform sampler2D ourTexture;
uniform float time;
uniform float selected;

void main()
{
    //fColor = vec4(0.5, 0.4, 0.8, 1.0);
	//fColor = texture(ourTexture, texCoord) + sin(6*time)*selected*vec4(1.0,1.0,1.0,1.0);
	fColor = fragColour + sin(6*time)*selected*vec4(1.0,1.0,1.0,1.0);
	//fColor = vec4(0.6,0.9,0.0,1.0);
	//fColor = vec4(texCoord,0.5,1.0);
	//fColor = fragColour;
}