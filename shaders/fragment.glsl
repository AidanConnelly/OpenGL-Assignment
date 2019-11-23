#version 450 core

out vec4 fColor;
in vec4 fragColour;
in vec3 vNorm;
in vec3 vPos;
in vec2 texCoord;

uniform sampler2D ourTexture;
uniform float time;
uniform float selected;
uniform float hasTexture;
uniform vec3 lightPos;
uniform float specExp;
uniform vec3 specCol;
uniform float opacity;
uniform vec3 ambCol;
void main()
{
    //fColor = vec4(0.5, 0.4, 0.8, 1.0);
	//fColor = texture(ourTexture, texCoord) + sin(6*time)*selected*vec4(1.0,1.0,1.0,1.0);
	vec3 normedVNorm = normalize(vNorm);
	vec3 posToLight = normalize(lightPos - vPos);
	float normLightDot = clamp(dot(normedVNorm,posToLight ),0,1);
	vec4 diffuse = fragColour;

	float specCoef = pow(normLightDot, specExp);
	vec4 specular = specCoef * vec4(specCol,1.0);
	vec4 ambient = 0.1 * vec4(ambCol,1.0);
	vec4 selected = hasTexture*texture(ourTexture, texCoord) + sin(6*time)*selected*vec4(1.0,1.0,1.0,1.0);
	fColor = diffuse + ambient + specular + selected;
	//fColor = vec4(0.6,0.9,0.0,1.0);
	//fColor = vec4(texCoord,0.5,1.0);
	//fColor = fragColour;
}