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

uniform isampler2D startMap;
uniform isampler2D endMap;
uniform isampler2D centroidMap;
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

#define NEIGHBOURS 1
#define SQRT_NUM_TRIAGS 1+2*NEIGHBOURS
#define NUMBER_OF_TRIANGLES_IN_TRIANGLE_LIST SQRT_NUM_TRIAGS*SQRT_NUM_TRIAGS

struct triangleShadowList{
    int index;
    vec2 starts[NUMBER_OF_TRIANGLES_IN_TRIANGLE_LIST];
    vec2 ends[NUMBER_OF_TRIANGLES_IN_TRIANGLE_LIST];
    vec2 centrds[NUMBER_OF_TRIANGLES_IN_TRIANGLE_LIST];
    float depths[NUMBER_OF_TRIANGLES_IN_TRIANGLE_LIST];
    int tIdxs[NUMBER_OF_TRIANGLES_IN_TRIANGLE_LIST];
};

#define MAX_UINT16 32767.0
#define MARGIN 0.5

float toFloat(int toConvert){
    float intInput = float(toConvert);
    float addedMargin = intInput/MAX_UINT16;
    float fn1to1 = addedMargin*(1.0+MARGIN);
    return fn1to1;
}

vec2 toFloatVec(ivec2 toConvert){
    vec2 tr = vec2(toFloat(toConvert.x), toFloat(toConvert.y));
//    fColor.r = tr.x;
//    fColor.b = tr.y;
    return tr;
}

triangleShadowList GetShadowsOnPoint(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
    // perform perspective divide
    vec3 fragProjCoords = fragPosLightSpace.xyz ;/// (-fragPosLightSpace.z);
    triangleShadowList list;
    list.index = 0;

    triangleShadowList undeduped;
    for (int x = -NEIGHBOURS;x<=+NEIGHBOURS;x+=1){
        for (int y = -NEIGHBOURS;y<=+NEIGHBOURS;y+=1){
            // transform to [0,1] range
            vec3 projCoord = fragProjCoords * 0.5 + 0.5;
            projCoord.x += (x) * (1.0/shadowMapSize);
            projCoord.y += (y) * (1.0/shadowMapSize);
            int i =(x+NEIGHBOURS)+(y+NEIGHBOURS)*SQRT_NUM_TRIAGS;

            // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
            ivec4 shadowTriangleIndexAsVec = texture(triangleMap, projCoord.xy);

            int cr = ((shadowTriangleIndexAsVec.r)  + 128);
            int cg = ((shadowTriangleIndexAsVec.g) + 128);
            int cb = ((shadowTriangleIndexAsVec.b) + 128);
            int ca = ((shadowTriangleIndexAsVec.a) + 128);

            int shadowTriangleIndex = (cr<<0) + (cg<<8) + (cb << 16) + (ca << 24);
            undeduped.tIdxs[i] = shadowTriangleIndex;
            undeduped.starts[i] = toFloatVec(texture(startMap, projCoord.xy).xy);
            undeduped.ends[i] = toFloatVec(texture(endMap, projCoord.xy).xy);
            undeduped.centrds[i] = toFloatVec(texture(centroidMap, projCoord.xy).xy);
            undeduped.depths[i] = texture(depthMap,projCoord.xy).r;
        }
    }

    for(int i = 0;i<NUMBER_OF_TRIANGLES_IN_TRIANGLE_LIST;i++){
        bool alreadyCounted = false;
        for(int j = 0;j<i;j++){
            if(undeduped.tIdxs[i]==undeduped.tIdxs[j]){
                alreadyCounted = true;
            }
		}
        if(!alreadyCounted){


            int idx = list.index;//list.index;
            list.starts[idx] = undeduped.starts[i].xy;
            list.ends[idx] = undeduped.ends[i].xy;
            list.centrds[idx] = undeduped.centrds[i].xy;
            list.tIdxs[idx] = undeduped.tIdxs[i];
            list.depths[idx] = undeduped.depths[i];
            list.index++;

        }
	}

    return list;
}

float shadowCoef(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir){
    vec3 projCoords = fragPosLightSpace.xyz;// / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float currentDepth = projCoords.z;

    triangleShadowList result = GetShadowsOnPoint(fragPosLightSpace, worldVPos, lightDir);

    float shadowCoef = 1.0;

    float anyNotInShadow = 0;
    float shadowCoefs[NUMBER_OF_TRIANGLES_IN_TRIANGLE_LIST];
    fColor = vec4(1.0,1.0,1.0,1.0);
    for (int i = 0;i<NUMBER_OF_TRIANGLES_IN_TRIANGLE_LIST;i++){
        shadowCoefs[i] = 1.0;
        if (i< result.index){
            float closestDepth = result.depths[i];
            float bias = 0.00080;



            if (currentDepth  - bias>closestDepth){
                //In shadow according to "bias" method

                vec2 start = -result.starts[i];
                vec2 end = -result.ends[i];

                vec2 centrd = -result.centrds[i];

                vec2 x = fragPosLightSpace.xy ;// (-fragPosLightSpace.z);

                vec2 along = start - end;
                vec2 alongNorm = normalize(along);
                vec2 centrdToStart = start - centrd;

                //1 = practically 0 degress, 0 = practically right angles
                float cosAngle_centrdToStart_alongNorm = dot(normalize(start - centrd),alongNorm);
                vec2 perpOut = centrdToStart - cosAngle_centrdToStart_alongNorm * alongNorm;
                vec2 perpOutNorm = normalize(perpOut);

                float amountAlong = -dot(x-start, alongNorm);
                float endAlong = length(along);
                vec2 closestPointOnLine = start + (-alongNorm * amountAlong);
                vec2 lineToX = x - closestPointOnLine;
                vec2 lineToXNorm = normalize(lineToX);
                bool shadow = (dot(lineToX-2*perpOutNorm,perpOutNorm)<0);//&&amountAlong>0&&amountAlong<endAlong;
                float adjustedDistanceFromLine = (length(lineToX)*shadowMapSize*0.5);
                float multIfInShadow = clamp(-1*adjustedDistanceFromLine,0.0,1.0);
                fColor *= shadow?multIfInShadow:1;

            }
            else{
                    anyNotInShadow = 1.0;
            }

        }
    }

    for(int i = 0;i<NUMBER_OF_TRIANGLES_IN_TRIANGLE_LIST;i++
    ){
//        shadowCoef *= shadowCoefs[i];
    }

    fColor *= shadowCoef;
    return shadowCoef;
}

void main()
{
    vec3 normedVNorm = normalize(vNorm);
    vec3 posToLight = normalize(lightPos - worldVPos);

    float dist = length(lightPos - worldVPos);
    float dist2rd = dist * dist;
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
//    fColor = diffuse + ambient + specular + selected;
}	