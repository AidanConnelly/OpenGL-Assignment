#version 450 core

out vec4 fColor;
in vec4 fragColour;
in vec3 vNorm;
in vec3 vPos;
in vec2 texCoord;

uniform mat4 m;
uniform mat4 vp;

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

	float dist = length(posToLight);

//	vec3 lightDir = normalize(lightPos - vertPos);
//	vec3 reflectDir = reflect(-lightDir, normal);
//	vec3 viewDir = normalize(-vertPos);
//
//	float lambertian = max(dot(lightDir,normal), 0.0);
//	specular = pow(specAngle, 4.0);

	vec3 viewDir = normalize(-vec3(vp*m*vec4(vPos,1.0)));
	vec3 reflectDir = reflect(-posToLight, normedVNorm);

	float viewDotReflect = clamp(dot(viewDir,reflectDir ),0,1);
	float diffuseCoef = clamp(dot(normedVNorm,posToLight),0,1);
	vec4 diffuse = diffuseCoef* fragColour;

	float specCoef = max(sign(diffuseCoef),0) * pow(viewDotReflect, specExp);
	vec4 specular = specCoef * vec4(specCol,1.0);
	vec4 ambient = 0.1 * vec4(ambCol,1.0);
	vec4 selected = hasTexture*texture(ourTexture, texCoord) + sin(6*time)*selected*vec4(1.0,1.0,1.0,1.0);
	fColor = diffuse + ambient + specular + selected;
//	fColor = vec4(diffuseCoef,specCoef,rename,1.0);
	//fColor = vec4(texCoord,0.5,1.0);
	//fColor = fragColour;
}