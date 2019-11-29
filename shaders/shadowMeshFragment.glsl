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
flat in float triangleIndex;

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

struct triangleShadow{
    vec4 start;
    vec4 end;
    vec4 centrd;
    float depth;
    float tIdx;
};

#define NUMBER_OF_TRIANGLES_IN_TRIANGLE_LIST 9

struct triangleShadowList{
    int index;
    vec2 starts[NUMBER_OF_TRIANGLES_IN_TRIANGLE_LIST];
    vec2 ends[NUMBER_OF_TRIANGLES_IN_TRIANGLE_LIST];
    vec2 centrds[NUMBER_OF_TRIANGLES_IN_TRIANGLE_LIST];
    float depths[NUMBER_OF_TRIANGLES_IN_TRIANGLE_LIST];
    int tIdxs[NUMBER_OF_TRIANGLES_IN_TRIANGLE_LIST];
};

triangleShadowList GetShadowsOnPoint(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
    // perform perspective divide
    vec3 fragProjCoords = fragPosLightSpace.xyz / (-fragPosLightSpace.z);
    triangleShadowList list;
    list.index = 0;

    int indexes[NUMBER_OF_TRIANGLES_IN_TRIANGLE_LIST];

    for (int x = -1;x<=+1;x+=1){
        for (int y = -1;y<=+1;y+=1){
            // transform to [0,1] range
            vec3 projCoords = fragProjCoords * 0.5 + 0.5;
            projCoords.x += (x) * (1.0/shadowMapSize);
            projCoords.y += (y) * (1.0/shadowMapSize);
            // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
            ivec4 shadowTriangleIndexAsVec = texture(triangleMap, projCoords.xy);


            int cr = ((shadowTriangleIndexAsVec.r)  + 128);
            int cg = ((shadowTriangleIndexAsVec.g) + 128);
            int cb = ((shadowTriangleIndexAsVec.b) + 128);
            int ca = ((shadowTriangleIndexAsVec.a) + 128);

            int shadowTriangleIndex = (cr<<0) + (cg<<8) + (cb << 16) + (ca << 24);
            indexes[x+y*3] = shadowTriangleIndex;
        }
    }

//	int positions[4];
//    for (int i = 0;i<4;i++){
//		int shadowTriangleIndex = indexes[i];
//		int pos = 0;
//		for (int j = 0;j<4;j++){
//			if (i!=j){
//				if (indexes[i]<indexes[j]){
//					pos++;
//				}
//			}
//		}
//		positions[i] = pos;
//    }

	int lastIndex = 0;
    for(int i = 0;i<NUMBER_OF_TRIANGLES_IN_TRIANGLE_LIST;i++){
        bool alreadyCounted = false;
        for(int j = 0;j<i;j++){
            if(indexes[i]==indexes[j]){
                alreadyCounted = true;
            }
		}
        if(!alreadyCounted){
            int x = (i % 3) - 1;
            int y = (i / 3) - 1;

            vec3 projCoords = fragProjCoords * 0.5 + 0.5;
            projCoords.x += (x) * (1.0/shadowMapSize);
            projCoords.y += (y) * (1.0/shadowMapSize);
            vec4 start = texture(startMap, projCoords.xy);
            vec4 end = texture(endMap, projCoords.xy);
            vec4 centrd = texture(centroidMap, projCoords.xy);
            int idx = list.index;//list.index;
            list.starts[idx] = start.xy;
            list.ends[idx] = end.xy;
            list.centrds[idx] = centrd.xy;
            list.tIdxs[idx] = indexes[i];
            list.depths[idx] = texture(depthMap, projCoords.xy).r;
            list.index++;

        }
	}

    return list;
}

float shadowCoef(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir){
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float currentDepth = projCoords.z;

    triangleShadowList result = GetShadowsOnPoint(fragPosLightSpace, worldVPos, lightDir);

    float shadowCoef = 1.0;

    float anyNotInShadow = 0;
    for (int i = 0;i<NUMBER_OF_TRIANGLES_IN_TRIANGLE_LIST;i++){
        if (i< result.index){
            float closestDepth = result.depths[i];
            float bias = 0.00010;



            if (currentDepth  - bias>closestDepth){
                //In shadow

                vec2 start = result.starts[i];
                vec2 end = result.ends[i];

                vec2 centrd = result.centrds[i];

                vec2 x = fragPosLightSpace.xy / (-fragPosLightSpace.z);

                vec2 along = end - start;
                vec2 alongNorm = normalize(along);
                vec2 closestPointOnLine = start + (alongNorm * (dot(x-start, alongNorm)));

                vec2 lineToX = x - closestPointOnLine;
                if(dot(lineToX,centrd-start)>0){
                    shadowCoef = 0;
                }
                else {
                    float dist = clamp(distance(x, closestPointOnLine)*shadowMapSize,0,1);
                    shadowCoef *= dist;
                }
                //
            }
            else{
                anyNotInShadow = 1;
            }
        }
    }

    shadowCoef *= anyNotInShadow;

    return shadowCoef;
}

void main()
{
    vec3 normedVNorm = normalize(vNorm);
    vec3 posToLight = normalize(lightPos - worldVPos);

    float dist = length(lightPos - worldVPos);
    float dist2rd = dist * dist;
    vec4 shadowVec4 = vec4(1.0, 1.0, 1.0, 1.0);//ShadowCalculation(FragPosLightSpace,worldVPos,-posToLight);
    float shadow = shadowCoef(FragPosLightSpace, worldVPos, -posToLight);
    float power = shadow * max((dot(posToLight, -lightDir)-(1-spotlightWidth))/spotlightWidth, 0) * lightPower/dist2rd;
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