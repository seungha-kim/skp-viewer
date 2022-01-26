#version 330 core

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

in vec3 normal;
in vec3 fragPos;
in vec4 fragPosLightSpace;
in vec2 frontTexCoord;
in vec2 backTexCoord;

uniform vec3 cameraPos;
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
    float bias = max(0.05 * (1.0 - dot(normal, -sunLightDir)), 0.005);
    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    return shadow;
}

void main() {
    vec3 norm = normalize(normal);
    vec3 toLight = -normalize(sunLightDir);
    vec3 lightColor = vec3(1.0);

    vec3 color; // TODO: vec4 alpha

    if (gl_FrontFacing) {
        color = frontColor;
        color = mix(color, vec3(texture(frontTexture, frontTexCoord)), frontTextureMix);
    } else {
        color = backColor;
        color = mix(color, vec3(texture(backTexture, backTexCoord)), backTextureMix);
    }

    float shadow = shadowMix * ShadowCalculation(fragPosLightSpace);

    FragColor = (1.0 - shadow) * vec4(color, 1.0);
}
