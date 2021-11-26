#pragma once
#include <glad/glad.h>

class ColorTexture {
public:
    ColorTexture(int width, int height);
    ~ColorTexture();
    void bind() const;
    [[nodiscard]] GLuint textureName() const;
    [[nodiscard]] int width() const;
    [[nodiscard]] int height() const;
private:
    GLuint m_textureName{};
    const int m_width;
    const int m_height;
};
