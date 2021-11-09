#pragma once

#include "BaseRenderer.h"
#include "Shader.h"
#include <glad/glad.h>

class TestRenderer: public BaseRenderer {
public:
    explicit TestRenderer(bool smile);
    ~TestRenderer() override;
    void render(RenderContext& ctx) override;
private:
    Shader ourShader;
    GLuint texture1, texture2;
    GLuint VAO, VBO, EBO;
};


