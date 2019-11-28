#version 450 core

out vec4 fColor;
in vec4 fragColour;
in vec3 vNorm;
in vec3 worldVPos;
in vec2 texCoord;
in vec4 FragPosLightSpace;

in vec3 tCentroid;
in vec3 tNormal;
in vec3 edgeAlong;
in vec3 edgePerpendicular;

uniform mat4 vp;
uniform vec3 cameraLocation;

uniform sampler2D startMap;
uniform sampler2D endMap;
uniform sampler2D centroidMap;
uniform sampler2D triangleMap;
uniform sampler2D depthMap;
uniform sampler2D ourTexture;

uniform float time;
uniform float selected;
uniform float hasTexture;

uniform vec3 lightPos;
uniform vec3 lightDir;
uniform float lightPower;
uniform float ambientLight;

uniform float specExp;
uniform vec3 specCol;
uniform float opacity;
uniform vec3 ambCol;

//0 for perfectly flat, 1 for 100% rough
uniform float specularSurfaceRoughness;


#define spotlightWidth 0.2

vec4 ShadowCalculation(vec4 fragPosLightSpace,vec3 normal, vec3 lightDir)
{
	// perform perspective divide
	vec3 projCoords = fragPosLightSpace.xyz / (-fragPosLightSpace.z);
	// transform to [0,1] range
	projCoords = projCoords * 0.5 + 0.5;
	// get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
	vec4 start = texture(startMap, projCoords.xy);
	vec4 end = texture(endMap, projCoords.xy);
	vec4 centrd = texture(centroidMap, projCoords.xy);
	float triangleIndex = texture(triangleMap, projCoords.xy).r;
	// get depth of current fragment from light's perspective
	float currentDepth = projCoords.z;
	// check whether current frag pos is in shadow
	float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
//	float shadow = currentDepth  - bias > closestDepth  ? 1.0 : 0.0;
	return vec4((triangleIndex==0)?0:1,0,0,1);//centrd;//vec4(projCoords.x, projCoords.y,0.0,1.0);
}

void main()
{
	vec3 normedVNorm = normalize(vNorm);
	vec3 posToLight = normalize(lightPos - worldVPos);

	float dist = length(lightPos - worldVPos);
	float dist2rd = dist * dist;
	vec4 shadowVec4 = ShadowCalculation(FragPosLightSpace,worldVPos,-posToLight);
	float shadow = 0.1;//ShadowCalculation(FragPosLightSpace,worldVPos,-posToLight);
	float power = (1-shadow) * max((dot(posToLight,-lightDir)-(1-spotlightWidth))/spotlightWidth,0) * lightPower/dist2rd;
	vec4 baseColour = fragColour;// + hasTexture*texture(ourTexture, texCoord);

	vec3 reflectDir = reflect(-posToLight, normedVNorm);
	vec3 viewDir = normalize(worldVPos - cameraLocation);
	float viewDotReflect = clamp(dot(-viewDir,reflectDir ),0,1);
	float diffuseCoef = clamp(dot(normedVNorm,posToLight),0,1);
	vec4 diffuse = power*diffuseCoef* baseColour;

	float viewable = clamp(diffuseCoef/(specularSurfaceRoughness+0.0000001),0,1);
	float specCoef = viewable * pow(viewDotReflect, specExp);
	vec4 specular = power*specCoef * vec4(specCol,1.0);
	vec4 ambient = ambientLight*baseColour*vec4(ambCol,1.0);
	vec4 selected = sin(6*time)*selected*vec4(1.0,1.0,1.0,1.0);
	fColor = shadowVec4;//texture(startMap,vec2(gl_FragCoord.x/100,gl_FragCoord.y/100));//shadowVec4;//diffuse + ambient + specular + selected;
}	