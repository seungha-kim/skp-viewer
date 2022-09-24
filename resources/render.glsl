#version 330 core

/**
# Table of contents

- Placeholder
- Definitions
- Common Functions
- Uniform Blocks
- Passes

# Pass variants

- MAIN
- DIRECTIONAL_LIGHT
- BOUNDING_BOX

# Stage variants

- VERT
- FRAG
*/

//PLACEHOLDER//
// will be replaced with variant definitions

/////////////////
// Definitions //
/////////////////

#if defined(VERT)
#define INOUT out
#elif defined(FRAG)
#define INOUT in
#endif

////////////////////
// Uniform Blocks //
////////////////////

// MUST be synchronized with `Uniforms.h`

layout (std140) uniform ViewBlock {
    mat4 viewMatrix;
    mat4 viewMatrixInverse;
    mat4 projectionMatrix;
    mat4 projectionMatrixInverse;
    mat4 viewProjectionMatrix;
    mat4 viewProjectionMatrixInverse;
    vec3 cameraPosition;
    float _pad1;
    vec3 cameraFront;
    float _pad2;
};

//////////////////////
// Common Functions //
//////////////////////

float map(float value, float min1, float max1, float min2, float max2) {
    return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}

///////////////
// Main Pass //
///////////////

#ifdef MAIN
INOUT vec3 normalWs;
INOUT vec3 normalVs;
INOUT vec3 fragPosWs;
INOUT vec3 fragPosVs;
INOUT vec4 fragPosLightSpace;
INOUT vec2 frontTexCoord;
INOUT vec2 backTexCoord;
flat INOUT float frontFacing;

// vert
uniform mat4 modelMatrix;
uniform mat4 lightSpaceMatrix;

// frag
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};
uniform vec3 sunLightDir;
uniform Material material;
uniform sampler2D shadowMap;
uniform float shadowMix;
uniform vec3 frontColor;
uniform vec3 backColor;
uniform sampler2D frontTexture;
uniform sampler2D backTexture;
uniform float frontTextureMix;
uniform float backTextureMix;
uniform int selected;
uniform float frontOpacity;
uniform float backOpacity;
#endif

#if defined(MAIN) && defined(VERT)
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aFaceNormal;
layout (location = 3) in vec2 aFrontTexCoord;
layout (location = 4) in vec2 aBackTexCoord;

void main() {
    // TODO: normal matrix outside of shader
    mat3 normalMatrix = mat3(transpose(inverse(modelMatrix)));
    normalWs = normalize(normalMatrix * aNormal);
    normalVs = (viewMatrix * vec4(normalWs, 0.0)).xyz;
    vec4 fragPosWs4 = modelMatrix * vec4(aPos, 1.0);
    fragPosWs = vec3(fragPosWs4);
    fragPosVs = vec3(viewMatrix * fragPosWs4);
    gl_Position = projectionMatrix * vec4(fragPosVs, 1.0);
    fragPosLightSpace = lightSpaceMatrix * vec4(fragPosWs, 1.0);

    vec3 faceNormalVs = normalize((viewMatrix * vec4(normalMatrix * aFaceNormal, 0.0)).xyz);
    frontFacing = sign(-dot(faceNormalVs, fragPosVs));

    frontTexCoord = aFrontTexCoord;
    backTexCoord = aBackTexCoord;
}
#endif

#if defined(MAIN) && defined(FRAG)
out vec4 FragColor;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    // float shadow = currentDepth > closestDepth  ? 1.0 : 0.0; -> shadow acne
    float bias = max(0.05 * (1.0 - dot(normalVs, -sunLightDir)), 0.005);
    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    return shadow;
}

void main() {
    if (selected > 0) {
        FragColor = vec4(1.0, 1.0, 0.0, 1.0);
        return;
    }
    vec3 norm = normalize(normalVs);
    vec3 toLight = -normalize(sunLightDir);
    vec3 lightColor = vec3(1.0);

    vec3 color; // TODO: vec4 alpha
    float opacity;

    if (frontFacing > 0) {
        color = frontColor;
        color = mix(color, vec3(texture(frontTexture, frontTexCoord)), frontTextureMix);
        opacity = frontOpacity;
    } else {
        color = backColor;
        color = mix(color, vec3(texture(backTexture, backTexCoord)), backTextureMix);
        opacity = backOpacity;
    }

    vec3 adjustedNormalWs = normalWs;
    if (dot(normalVs, -fragPosVs) < 0)
    {
        // SketchUp 뒷면에 대해서도 문제없이 렌더링을 하기 위한 workaround
        adjustedNormalWs = -adjustedNormalWs;
    }
    float shading = 0.5 + clamp(dot(-adjustedNormalWs, cameraFront) / 2, 0, 1);
    color *= shading;
    float shadow = shadowMix * ShadowCalculation(fragPosLightSpace);

    FragColor = vec4(color, opacity);
}
#endif

///////////////////////
// Directional Light //
///////////////////////

#ifdef DIRECTIONAL_LIGHT
uniform mat4 dirLightViewProjectionMatrix;
uniform vec3 color;
uniform mat4 modelMatrix;
#endif

#if defined(DIRECTIONAL_LIGHT) && defined(VERT)
layout (location = 0) in vec3 aPos;

void main() {
    gl_Position = dirLightViewProjectionMatrix * modelMatrix * vec4(aPos, 1.0);
}
#endif

#if defined(DIRECTIONAL_LIGHT) && defined(FRAG)
out vec4 FragColor;

void main() {
    FragColor = vec4(color, 1.0);
}
#endif

//////////////////
// Bounding Box //
//////////////////

#ifdef BOUNDING_BOX
uniform mat4 modelMatrix;
#endif

#if defined(BOUNDING_BOX) && defined(VERT)
layout (location = 0) in vec3 aPos;

void main() {
    gl_Position = viewProjectionMatrix * modelMatrix * vec4(aPos, 1.0);
}
#endif

#if defined(BOUNDING_BOX) && defined(FRAG)
out vec4 FragColor;

void main() {
    FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
#endif

/////////////////////
// Geometry Buffer //
/////////////////////

#ifdef G_BUFFER
uniform mat4 modelMatrix;
uniform mat3 normalMatrix; // mat3(transpose(inverse(modelMatrix)))

INOUT vec3 wsPos;
INOUT vec3 wsVertexNormal;
INOUT vec3 wsFaceNormal;
#endif

#if defined(G_BUFFER) && defined(VERT)
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aVertexNormal;
layout (location = 2) in vec3 aFaceNormal;

void main() {
    wsPos = (modelMatrix * vec4(aPos, 1)).xyz;
    wsVertexNormal = normalize(normalMatrix * aVertexNormal);
    wsFaceNormal = normalize(normalMatrix * aFaceNormal);

    // workaround for flipped normals in skp
    vec3 vsPos = (viewMatrix * vec4(wsPos, 1.0)).xyz;
    vec3 vsVertexNormal = mat3(viewMatrix) * wsVertexNormal;
    vec3 vsFaceNormal = mat3(viewMatrix) * wsFaceNormal;
    if (dot(vsPos, vsVertexNormal) > 0) {
        wsVertexNormal = -wsVertexNormal;
    }
    if (dot(vsPos, vsFaceNormal) > 0) {
        wsFaceNormal = -wsFaceNormal;
    }

    gl_Position = viewProjectionMatrix * modelMatrix * vec4(aPos, 1.0);
}
#endif

#if defined(G_BUFFER) && defined(FRAG)
layout (location = 0) out vec3 FragVertexNormal;
layout (location = 1) out vec3 FragFaceNormal;

void main() {
    FragVertexNormal = wsVertexNormal;
    FragFaceNormal = wsFaceNormal;
}
#endif
