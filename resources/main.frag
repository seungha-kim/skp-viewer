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

uniform vec3 cameraPos;
uniform vec3 sunLightDir;
uniform Material material;
uniform sampler2D shadowMap;
uniform float shadowMix;

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

    // diffuse
    float diff = max(dot(norm, toLight), 0.0);
    vec3 diffuseColor = (diff * material.diffuse) * lightColor;

    // specular
    vec3 toFrag = normalize(cameraPos - fragPos);
    vec3 reflection = reflect(toLight, norm);
    float spec = pow(max(dot(toFrag, -reflection), 0.0), material.shininess);
    vec3 specularColor = material.specular * spec * lightColor;

    // ambient
    vec3 ambientColor = lightColor * material.ambient;

    // shadow
    float shadow = shadowMix * ShadowCalculation(fragPosLightSpace);

    vec3 result = (1.0 - shadow) * (diffuseColor + specularColor) + ambientColor;
    FragColor = vec4(result, 1.0);
}
