#version 330 core

in vec3 normal;
in vec3 fragPos;

uniform vec3 cameraPos;
uniform vec3 lightPos;

out vec4 FragColor;

void main() {
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - fragPos);
    vec3 lightColor = vec3(1.0);

    // diffuse
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuseColor = diff * lightColor;

    // specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(cameraPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 128);
    vec3 specularColor = specularStrength * spec * lightColor;

    // ambient
    vec3 ambientColor = vec3(0.2);

    vec3 result = vec3(0.3, 0.2, 0.1) * (diffuseColor + specularColor + ambientColor);
    FragColor = vec4(result, 1.0);
}
