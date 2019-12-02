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
flat in int triangleIndex;

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

#define NEIGHBOURS 1

#define SQRT_NUM_TRIAGS (1+2*NEIGHBOURS)
#define NUMBER_OF_TRIANGLES_IN_TRIANGLE_LIST (SQRT_NUM_TRIAGS*SQRT_NUM_TRIAGS)

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
    vec3 fragProjCoords = fragPosLightSpace.xyz ;/// (-fragPosLightSpace.z);
    triangleShadowList list;
    list.index = 0;

    int indexes[NUMBER_OF_TRIANGLES_IN_TRIANGLE_LIST];

    for (int x = -NEIGHBOURS;x<=+NEIGHBOURS;x+=1){
        for (int y = -NEIGHBOURS;y<=+NEIGHBOURS;y+=1){
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
            indexes[(x+NEIGHBOURS)+(y+NEIGHBOURS)*SQRT_NUM_TRIAGS] = shadowTriangleIndex;
        }
    }

    for(int i = 0;i<NUMBER_OF_TRIANGLES_IN_TRIANGLE_LIST;i++){
        bool alreadyCounted = false;
        for (int j = 0;j<i;j++){
            if (indexes[i]==indexes[j]){
                alreadyCounted = true;
            }
		}
        if(!alreadyCounted){
            int x = (i % SQRT_NUM_TRIAGS)- NEIGHBOURS;
            int y = (i / SQRT_NUM_TRIAGS) - NEIGHBOURS;

//            if((x+NEIGHBOURS)+(y+NEIGHBOURS)*SQRT_NUM_TRIAGS!=i){
//                fColor = vec4(0.5,1.0,0.5,1.0);
//                list.index = -1;
//                return list;
//            }

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
    vec3 projCoords = fragPosLightSpace.xyz;// / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float currentDepth = projCoords.z;

    triangleShadowList result = GetShadowsOnPoint(fragPosLightSpace, worldVPos, lightDir);

    float shadowCoef = 1.0;

    float anyNotInShadow = 0;
    fColor = vec4(1.0,1.0,1.0,1.0);
    int t = 0;
    for (int i = 0;i<NUMBER_OF_TRIANGLES_IN_TRIANGLE_LIST;i++){
        if (i< result.index){
            float closestDepth = result.depths[i];
            float bias = 0.00080;



            if (currentDepth  - bias>closestDepth && triangleIndex!= result.tIdxs[i]){
                t++;
                //In shadow according to "bias" method

                vec2 start = result.starts[i];
                vec2 end = result.ends[i];

                vec2 centrd = result.centrds[i];

                vec2 x = fragPosLightSpace.xy ;// (-fragPosLightSpace.z);

                vec2 along = start - end;
                vec2 alongNorm = normalize(along);
                vec2 centrdToStart = start - centrd;

                mat2 mtrx = mat2(0,-1,+1,0);
                vec2 perpOutNorm = -mtrx*alongNorm;

                float amountAlong = -dot(x-start, alongNorm);
                float endAlong = length(along);
                vec2 closestPointOnLine = start + (-alongNorm * amountAlong);
                vec2 lineToX = x - closestPointOnLine;
                float borderDist = 0.7;
                vec2 borderToX = x - (closestPointOnLine + borderDist* perpOutNorm/shadowMapSize);
                bool shadow1 =(dot(borderToX,perpOutNorm)<0)&&amountAlong>0&&amountAlong<endAlong;
                bool shadow2 =(dot(lineToX,perpOutNorm)<0)&&amountAlong>0&&amountAlong<endAlong;
                float adjustedDistanceFromBorder = (1/(borderDist))*length(borderToX)*shadowMapSize;
                float multIfInShadow = clamp(1.0-1.0*adjustedDistanceFromBorder, 0.0,1.0);
                fColor.r *= shadow1?0:1;
                fColor.b *= shadow2?0:1;
                fColor.g *= multIfInShadow;

                }
                else {
                    anyNotInShadow = 1.0;
                }

        }
    }

//    fColor.r = (1.0/256.0)*(((result.tIdxs[0]>>0) %256) - 128);
//    fColor.g = (1.0/256.0)*(((result.tIdxs[0]>>8)%256) - 128);
//    fColor.b = (1.0/256.0)*(((result.tIdxs[0]>>16)%256) - 128);
//    if(result.index>=2){
////        fColor.r = (1.0/256.0)*(((result.tIdxs[1]>>0) %256) - 128);
//        fColor.g = (1.0/256.0)*(((result.tIdxs[1]>>8)%256) - 128);
//        fColor.b = (1.0/256.0)*(((result.tIdxs[1]>>16)%256) - 128);
//    }
////    fColor.g = 0.3 * t;
//    fColor.b = (result.tIdxs[0] == triangleIndex)?1.0:0.0;
//    fColor.r = (result.tIdxs[1] == triangleIndex)?1.0:0.0;
//    fColor.b = (result.tIdxs[0] == result.tIdxs[1])?1.0:0.0;

    for(int i = 0;i<NUMBER_OF_TRIANGLES_IN_TRIANGLE_LIST;i++
    ){
//        shadowCoef *= shadowCoefs[i];
    }

//    fColor *= shadowCoef;
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
