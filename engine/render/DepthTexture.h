#pragma once
#include <glad/glad.h>

class DepthTexture {
public:
    DepthTexture(int width, int height);
    ~DepthTexture();
    [[nodiscard]] GLuint textureName() const;
    [[nodiscard]] int width() const;
    [[nodiscard]] int height() const;
private:
    GLuint m_textureName{};
    const int m_width;
    const int m_height;
};
