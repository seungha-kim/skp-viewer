#pragma once

#include "CameraState.h"
#include "Shader.h"
#include <glad/glad.h>

struct RenderContext {
    const CameraState &cam;
    const float playbackValue;
};

class Renderer {
public:
    Renderer();
    ~Renderer();
    void render(RenderContext& ctx);
private:
    Shader ourShader;
    GLuint texture1, texture2;
    GLuint VAO, VBO, EBO;
};


