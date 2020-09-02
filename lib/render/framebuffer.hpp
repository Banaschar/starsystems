#ifndef FRAMEBUFFER_HPP
#define FRAMEBUFFER_HPP

#include "oglheader.hpp"

class FrameBuffer {
public:
    static void destroyFrameBuffer(unsigned int *frameBuffer) {
        glDeleteFramebuffers(1, frameBuffer);
    }
    static void destroyRenderBuffer(unsigned int *renderBuffer) {
        glDeleteRenderbuffers(1, renderBuffer);
    }

    static void destroyTexture(unsigned int *texture) {
        glDeleteTextures(1, texture);
    }
    static unsigned int createFrameBuffer() {
        unsigned int id;
        glGenFramebuffers(1, &id);
        glBindFramebuffer(GL_FRAMEBUFFER, id);
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
        return id;
    }

    static unsigned int createTextureAttachment(int width, int height) {
        unsigned int textureId;
        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, textureId, 0);

        return textureId;
    }

    static unsigned int createDepthTextureAttachment(int width, int height) {
        unsigned int textureId;
        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, textureId, 0);

        return textureId;
    }

    static unsigned int createDepthBufferAttachment(int width, int height) {
        unsigned int depthBuf;
        glGenRenderbuffers(1, &depthBuf);
        glBindRenderbuffer(GL_RENDERBUFFER, depthBuf);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuf);

        return depthBuf;
    }

    static void bindFrameBuffer(unsigned int frameBuffer, int width, int height) {
        glBindTexture(GL_TEXTURE_2D, 0);    // TODO: Do I need this?
        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glViewport(0, 0, width, height);
    }

    static void unbindActiveFrameBuffer(int width, int height) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, width, height);
    }
};
#endif