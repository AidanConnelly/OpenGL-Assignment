#version 450 core

out vec4 fColor;
in vec4 fragColour;
in vec2 texCoord;

uniform sampler2D ourTexture;

void main()
{
    //fColor = vec4(0.5, 0.4, 0.8, 1.0);
	fColor = texture(ourTexture, texCoord);
	//fColor = vec4(0.6,0.9,0.0,1.0);
	//fColor = vec4(texCoord,0.5,1.0);
	//fColor = fragColour;
}