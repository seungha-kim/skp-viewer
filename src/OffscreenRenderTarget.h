#pragma once
#include <glad/glad.h>
#include "ColorTexture.h"
#include <memory>
#include <glm/glm.hpp>
#include "DepthTexture.h"

class OffscreenRenderTargetBinding;
class OffscreenRenderTarget {
    friend class OffscreenRenderTargetBinding;
public:
    OffscreenRenderTarget();
    ~OffscreenRenderTarget();
    void setTargetColorTexture(const ColorTexture& texture, int index);
    void setTargetDepthTexture(const DepthTexture& texture);
    void setTargetColorTextureCount(int count);
    [[nodiscard]] OffscreenRenderTargetBinding bindAndPrepare(glm::vec3 clearColor, int viewportWidth, int viewportHeight);
    void checkState();
private:
    GLuint m_fbo{};
    bool m_depthGiven = false;
    bool m_colorGiven = false;

    void unbind();
};

class OffscreenRenderTargetBinding {
public:
    explicit OffscreenRenderTargetBinding(OffscreenRenderTarget& target);
    ~OffscreenRenderTargetBinding();
private:
    OffscreenRenderTarget& m_target;
};
