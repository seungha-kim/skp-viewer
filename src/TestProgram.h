#pragma once

#include "Program.h"
#include "Shader.h"
#include "RenderContext.h"
#include <glad/glad.h>

class TestProgram: public Program {
public:
    explicit TestProgram(bool smile);
    ~TestProgram() override;
    void render(RenderContext& ctx) override;
private:
    Shader ourShader;
    GLuint texture1, texture2;
    GLuint VAO, VBO, EBO;
};


