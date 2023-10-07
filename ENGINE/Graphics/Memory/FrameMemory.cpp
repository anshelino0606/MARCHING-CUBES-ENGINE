//
// Created by Anhelina Modenko on 06.10.2023.
//

#include "FrameMemory.h"

FrameBuffer::FrameBuffer()
        : val(0), width(0), height(0), bitCombo(0) {}

FrameBuffer::FrameBuffer(GLuint width, GLuint height, GLbitfield bitCombo)
: val(0), width(width), height(height), bitCombo(bitCombo) {}

void FrameBuffer::generate() {
    glGenFramebuffers(1, &val);
}

void FrameBuffer::disableColorBuffer() {
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
}

void FrameBuffer::bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, val);
}

void FrameBuffer::setViewport() {
    glViewport(0, 0, width, height);
}

void FrameBuffer::clear() {
    glClear(bitCombo);
}

void FrameBuffer::activate() {
    bind();
    setViewport();
    clear();
}

void FrameBuffer::activateRBO(GLenum attachType, GLenum format) {
    GLuint rbo;

    // generate
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);

    // attach
    glRenderbufferStorage(GL_RENDERBUFFER, format, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachType, GL_RENDERBUFFER, rbo);

    // add to list
    rbos.push_back(rbo);
}

void FrameBuffer::activateTexture(GLenum attachType, GLenum format, GLenum type) {
    std::string name = &"tex" [ textures.size()];
    Texture tex(name);

    // allocate
    tex.bind();
    tex.allocate(format, width, height, type);
    Texture::setParams(GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);

    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    // attach
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachType, GL_TEXTURE_2D, tex.id, 0);

    textures.push_back(tex);
}

void FrameBuffer::attachTexture(GLenum attachType, Texture tex) {
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachType, GL_TEXTURE_2D, tex.id, 0);
}

void FrameBuffer::activateCubemap(GLenum attachType, GLenum format, GLenum type) {
    cubemap = Cubemap();

    cubemap.generate();
    cubemap.bind();
    cubemap.allocate(format, width, height, type);

    glFramebufferTexture(GL_FRAMEBUFFER, attachType, cubemap.id, 0);
}

void FrameBuffer::cleanup() {
    // delete RBOs
    glDeleteRenderbuffers(rbos.size(), &rbos[0]);

    // delete generated textures
    for (Texture t : textures) {
        t.cleanup();
    }

    // delete FBO
    glDeleteFramebuffers(1, &val);
}

void RenderBuffer::generate() {
    glGenRenderbuffers(1, &val);
}

void RenderBuffer::bind() {
    glBindRenderbuffer(GL_RENDERBUFFER, val);
}

void RenderBuffer::allocate(GLenum format, GLuint width, GLuint height) {
    glRenderbufferStorage(GL_RENDERBUFFER, format, width, height);
}

void RenderBuffer::cleanup() {
    glDeleteRenderbuffers(1, &val);
}