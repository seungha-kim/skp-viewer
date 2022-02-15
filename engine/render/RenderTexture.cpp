#include "RenderTexture.h"
#include <glad/glad.h>

namespace acon {

// TODO: RGBA, BGRA 처리 좀 더 깔끔하게. SketchUp SDK 의 스펙때문에 이렇게 된 것으로 기억.
#ifdef WIN32
#define PIXEL_FORMAT GL_BGRA
#else
#define PIXEL_FORMAT GL_RGBA
#endif

RenderTexture::RenderTexture(std::unique_ptr<TextureData>&& textureData)
        : m_tempData(std::move(textureData)) { }

RenderTexture::~RenderTexture() {
    if (m_textureName) {
        glDeleteTextures(1, &m_textureName);
    }
}

GLuint RenderTexture::textureName() const {
    return m_textureName;
}

void RenderTexture::prepareToRender() {
    if (!m_tempData)
        return;
    glGenTextures(1, &m_textureName);
    glBindTexture(GL_TEXTURE_2D, m_textureName);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        m_tempData->width(),
        m_tempData->height(),
        0,
        PIXEL_FORMAT,
        GL_UNSIGNED_BYTE,
        m_tempData->data());
    glBindTexture(GL_TEXTURE_2D, 0);

    m_tempData = nullptr;
}

}
