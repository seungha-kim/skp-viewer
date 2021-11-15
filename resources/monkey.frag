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
uniform vec3 lightPos;
uniform Material material;

out vec4 FragColor;

void main() {
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - fragPos);
    vec3 lightColor = vec3(1.0);

    // diffuse
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuseColor = (diff * material.diffuse) * lightColor;

    // specular
    vec3 viewDir = normalize(cameraPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specularColor = material.specular * spec * lightColor;

    // ambient
    vec3 ambientColor = lightColor * material.ambient;

    vec3 result = diffuseColor + specularColor + ambientColor;
    FragColor = vec4(result, 1.0);
}
