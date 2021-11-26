#include "OffscreenRenderTarget.h"

OffscreenRenderTarget::OffscreenRenderTarget() {
    glGenFramebuffers(1, &m_fbo);
}

OffscreenRenderTarget::~OffscreenRenderTarget() {
    glDeleteFramebuffers(1, &m_fbo);
}

OffscreenRenderTargetBinding OffscreenRenderTarget::bindAndPrepare(glm::vec3 clearColor, int viewportWidth, int viewportHeight) {
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glViewport(0, 0, viewportWidth, viewportHeight);
    glClearColor(clearColor.r, clearColor.g, clearColor.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!m_colorGiven) {
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    }

    return OffscreenRenderTargetBinding{*this};
}

void OffscreenRenderTarget::setTargetColorTexture(const ColorTexture &texture) {
    m_colorGiven = true;
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glFramebufferTexture(
            GL_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT0,
            texture.textureName(),
            0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OffscreenRenderTarget::setTargetDepthTexture(const DepthTexture &texture) {
    m_depthGiven = true;
    // https://stackoverflow.com/questions/5751081/how-to-use-depth-testing-when-rendering-to-an-offscreen-buffer-then-onto-texture
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glFramebufferTexture(
            GL_FRAMEBUFFER,
            GL_DEPTH_ATTACHMENT,
            texture.textureName(),
            0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OffscreenRenderTarget::unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OffscreenRenderTarget::checkState() {
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        exit(1);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

OffscreenRenderTargetBinding::OffscreenRenderTargetBinding(OffscreenRenderTarget &target): m_target{target} {}

OffscreenRenderTargetBinding::~OffscreenRenderTargetBinding() {
    m_target.unbind();
}
