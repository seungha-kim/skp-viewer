#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aFaceNormal;
layout (location = 3) in vec2 aFrontTexCoord;
layout (location = 4) in vec2 aBackTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

out vec3 normal;
out vec3 fragPos;
out vec4 fragPosLightSpace;
out vec2 frontTexCoord;
out vec2 backTexCoord;

float map(float value, float min1, float max1, float min2, float max2) {
    return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}

void main() {
    // TODO: normal matrix outside of shader
    mat3 normalMatrix = mat3(transpose(inverse(view * model)));
    normal = normalize(normalMatrix * aNormal);

    fragPos = vec3(model * vec4(aPos, 1.0));
    gl_Position = projection * view * vec4(fragPos, 1.0);
    fragPosLightSpace = lightSpaceMatrix * vec4(fragPos, 1.0);

    frontTexCoord = aFrontTexCoord;
    backTexCoord = aBackTexCoord;
}
