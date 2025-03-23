#include "fbo.hpp"

FBO::FBO(unsigned int width, unsigned int height, GLenum target) {
    glGenFramebuffers(1, &m_id);
    bind(GL_FRAMEBUFFER);

    g_Albedo = std::make_unique<Texture>(width, height, target, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, target, g_Albedo->getID(), 0);

    unsigned int attachments[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, attachments);
}

void FBO::bind(GLenum target) {
    glBindFramebuffer(target, m_id);
}

void FBO::blit(int width, int height, GLenum source) {
    glReadBuffer(source);
    glDrawBuffer(source);
    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);    
}

void FBO::refactor(unsigned int width, unsigned int height, GLenum target) {
    bind(GL_FRAMEBUFFER);

    if (g_Albedo)
        g_Albedo->destroy();
    
    g_Albedo = std::make_unique<Texture>(width, height, target, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, target, g_Albedo->getID(), 0);

    unbind();
}

void FBO::unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FBO::destroy() {
    g_Albedo->destroy();
    glDeleteFramebuffers(1, &m_id);
}