#version 450 core

out vec4 fColor;
in vec4 fragColour;
in vec3 vNorm;
in vec3 worldVPos;
in vec2 texCoord;
in vec4 FragPosLightSpace;

uniform mat4 vp;
uniform vec3 cameraLocation;

uniform sampler2D startMap;
uniform sampler2D endMap;
uniform sampler2D centroidMap;
uniform isampler2D triangleMap;
uniform sampler2D depthMap;
uniform sampler2D ourTexture;

uniform float shadowMapSize;

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

    void main()
{
    vec3 normedVNorm = normalize(vNorm);
    vec3 posToLight = normalize(lightPos - worldVPos);

    float dist = length(lightPos - worldVPos);
    float dist2rd = dist * dist;
    vec4 shadowVec4 = vec4(1.0, 1.0, 1.0, 1.0);
    float power = max((dot(posToLight, -lightDir)-(1-spotlightWidth))/spotlightWidth, 0) * lightPower/dist2rd;
    vec4 baseColour = fragColour + hasTexture*texture(ourTexture, texCoord);

    vec3 reflectDir = reflect(-posToLight, normedVNorm);
    vec3 viewDir = normalize(worldVPos - cameraLocation);
    float viewDotReflect = clamp(dot(-viewDir, reflectDir), 0, 1);
    float diffuseCoef = clamp(dot(normedVNorm, posToLight), 0, 1);
    vec4 diffuse = power*diffuseCoef* baseColour;

    float viewable = clamp(diffuseCoef/(specularSurfaceRoughness+0.0000001), 0, 1);
    float specCoef = viewable * pow(viewDotReflect, specExp);
    vec4 specular = power*specCoef * vec4(specCol, 1.0);
    vec4 ambient = ambientLight*baseColour*vec4(ambCol, 1.0);
    vec4 selected = sin(6*time)*selected*vec4(1.0, 1.0, 1.0, 1.0);
    fColor = diffuse + ambient + specular + selected;
}	