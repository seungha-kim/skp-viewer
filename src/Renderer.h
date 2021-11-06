#pragma once

#include "CameraState.h"
#include "Shader.h"
#include <glad/glad.h>

class Renderer {
public:
    Renderer();
    ~Renderer();
    void render(const CameraState &cameraState, float playbackValue);
private:
    Shader ourShader;
    GLuint texture1, texture2;
    GLuint VAO, VBO, EBO;
};


