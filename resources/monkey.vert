#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

out vec3 normal;
out vec3 fragPos;
out vec4 fragPosLightSpace;

void main() {
    normal = aNormal; // TODO: skewed normal
    fragPos = vec3(model * vec4(aPos, 1.0));
    gl_Position = projection * view * vec4(fragPos, 1.0);
    fragPosLightSpace = lightSpaceMatrix * vec4(fragPos, 1.0);
}