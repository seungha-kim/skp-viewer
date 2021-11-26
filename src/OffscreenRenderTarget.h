#pragma once
#include <glad/glad.h>
#include "ColorTexture.h"
#include <memory>
#include <glm/glm.hpp>
#include "DepthTexture.h"

class OffscreenRenderTarget {
public:
    OffscreenRenderTarget();
    ~OffscreenRenderTarget();
    void setTargetColorTexture(const ColorTexture& texture);
    void setTargetDepthTexture(const DepthTexture& texture);
    void bindAndPrepare(glm::vec3 clearColor, int viewportWidth, int viewportHeight) const;
    void unbind();
    void checkState();
private:
    GLuint m_fbo{};
    bool m_depthGiven = false;
    bool m_colorGiven = false;
};
