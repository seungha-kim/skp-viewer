#version 330 core

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

in vec3 normal;
in vec3 fragPos;

uniform vec3 cameraPos;
uniform vec3 sunLightDir;
uniform Material material;

out vec3 color;

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

    vec3 result = diffuseColor + specularColor + ambientColor;
    color = result;
}
