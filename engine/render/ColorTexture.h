#pragma once
#include <glad/glad.h>

namespace acon {

class ColorTexture {
public:
    ColorTexture(int width, int height);
    ~ColorTexture();
    [[nodiscard]] GLuint textureName() const;
    [[nodiscard]] int width() const;
    [[nodiscard]] int height() const;

private:
    GLuint m_textureName {};
    const int m_width;
    const int m_height;
};

}
