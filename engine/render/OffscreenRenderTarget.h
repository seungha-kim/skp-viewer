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
    // TODO: OffscreenRenderTargetBinding 를 movable 로 만들고 불필요한 힙 할당 제거하기
    [[nodiscard]] std::unique_ptr<OffscreenRenderTargetBinding> bind();
    [[nodiscard]] std::unique_ptr<OffscreenRenderTargetBinding> bindAndPrepare(glm::vec3 clearColor, int viewportWidth, int viewportHeight);
    void checkState();
private:
    GLuint m_fbo{};
    bool m_depthGiven = false;
    bool m_colorGiven = false;
};

class OffscreenRenderTargetBinding {
public:
    explicit OffscreenRenderTargetBinding(OffscreenRenderTarget& target);
    ~OffscreenRenderTargetBinding();
private:
    GLint m_prevFbo = 0;
};