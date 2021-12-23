#include "OffscreenRenderTarget.h"
#include "checkError.h"

OffscreenRenderTarget::OffscreenRenderTarget() {
    glGenFramebuffers(1, &m_fbo);
}

OffscreenRenderTarget::~OffscreenRenderTarget() {
    glDeleteFramebuffers(1, &m_fbo);
}

std::unique_ptr<OffscreenRenderTargetBinding> OffscreenRenderTarget::bind() {
    return std::make_unique<OffscreenRenderTargetBinding>(*this);
}

std::unique_ptr<OffscreenRenderTargetBinding> OffscreenRenderTarget::bindAndPrepare(glm::vec3 clearColor, int viewportWidth, int viewportHeight) {
    auto binding = bind();
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glViewport(0, 0, viewportWidth, viewportHeight);
    glClearColor(clearColor.r, clearColor.g, clearColor.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!m_colorGiven) {
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    }

    return binding;
}

void OffscreenRenderTarget::setTargetColorTexture(const ColorTexture &texture, int index) {
    m_colorGiven = true;
    auto binding = bind();
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glFramebufferTexture(
            GL_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT0 + index,
            texture.textureName(),
            0);
}

void OffscreenRenderTarget::setTargetDepthTexture(const DepthTexture &texture) {
    m_depthGiven = true;
    auto binding = bind();
    // https://stackoverflow.com/questions/5751081/how-to-use-depth-testing-when-rendering-to-an-offscreen-buffer-then-onto-texture
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glFramebufferTexture(
            GL_FRAMEBUFFER,
            GL_DEPTH_ATTACHMENT,
            texture.textureName(),
            0);
}

void OffscreenRenderTarget::checkState() {
    auto binding = bind();
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        exit(1);
    }
}

void OffscreenRenderTarget::setTargetColorTextureCount(int count) {
    // TODO: setTargetColorTexture 만 가지고도 가능
    auto binding = bind();
    static GLuint attachments[] {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT0 + 1, GL_COLOR_ATTACHMENT0 + 2, GL_COLOR_ATTACHMENT0 + 3};
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glDrawBuffers(count, attachments);
}

OffscreenRenderTargetBinding::OffscreenRenderTargetBinding(OffscreenRenderTarget &target) {
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &m_prevFbo);
}

OffscreenRenderTargetBinding::~OffscreenRenderTargetBinding() {
    glBindFramebuffer(GL_FRAMEBUFFER, m_prevFbo);
}
