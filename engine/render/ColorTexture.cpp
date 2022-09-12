#include "ColorTexture.h"

namespace acon {

ColorTexture::ColorTexture(int width, int height, GLint format)
        : m_width(width)
        , m_height(height) {
    glGenTextures(1, &m_textureName);
    glBindTexture(GL_TEXTURE_2D, m_textureName);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);
}

ColorTexture::~ColorTexture() {
    glDeleteTextures(1, &m_textureName);
}

GLuint ColorTexture::textureName() const {
    return m_textureName;
}

int ColorTexture::width() const {
    return m_width;
}

int ColorTexture::height() const {
    return m_height;
}

}
