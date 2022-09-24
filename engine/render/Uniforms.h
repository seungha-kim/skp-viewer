#pragma once

#include <glm/glm.hpp>

using namespace glm;

struct ViewBlock {
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
