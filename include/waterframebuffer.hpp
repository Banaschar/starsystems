#ifndef WATERFRAMEBUFFER_H
#define WATERFRAMEBUFFER_H

#include "global.hpp"

#define REFLECTION_WIDTH 320
#define REFLECTION_HEIGHT 180
#define REFRACTION_WIDTH 1280
#define REFRACTION_HEIGHT 720

class WaterFrameBuffer {
public:
    WaterFrameBuffer(int windowWitdh, int windowHeight) : 
                    windowWitdh_(windowWitdh), windowHeight_(windowHeight){
        reflectionWidth_ = REFLECTION_WIDTH;
        reflectionHeight_ = REFLECTION_HEIGHT;
        refractionWidth_ = REFRACTION_WIDTH;
        refractionHeight_ = REFRACTION_HEIGHT;
        initReflectionFrameBuffer();
        initRefractionFrameBuffer();
    }
    ~WaterFrameBuffer() {
        glDeleteFramebuffers(1, &reflectionFb_);
        glDeleteTextures(1, &reflectionTexture_);
        glDeleteRenderbuffers(1, &reflectionDepthBuffer_);
        glDeleteFramebuffers(1, &refractionFb_);
        glDeleteTextures(1, &refractionTexture_);
        glDeleteTextures(1, &refractionDepthTexture_);
    }

    unsigned int getReflectionTexture() {
        return reflectionTexture_;
    }

    unsigned int getRefractionTexture_() {
        return refractionTexture_;
    }

    unsigned int getRefractionDepthTexture() {
        return refractionDepthTexture_;
    }

    void unbindActiveFrameBuffer() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, windowWitdh_, windowHeight_);
    }

    void bindReflectionFrameBuffer() {
        bindFrameBuffer(reflectionFb_, reflectionWidth_, reflectionHeight_);
    }

    void bindRefractionFrameBuffer() {
        bindFrameBuffer(refractionFb_, refractionWidth_, refractionHeight_);
    }

private:
    unsigned int reflectionFb_;
    unsigned int reflectionTexture_;
    unsigned int reflectionDepthBuffer_;

    unsigned int refractionFb_;
    unsigned int refractionTexture_;
    unsigned int refractionDepthTexture_;
    int reflectionWidth_, reflectionHeight_, refractionHeight_, refractionWidth_;
    int windowWitdh_, windowHeight_;

    void initReflectionFrameBuffer() {
        reflectionFb_ = createFrameBuffer();
        reflectionTexture_ = createTextureAttachment(reflectionWidth_, reflectionHeight_);
        reflectionDepthBuffer_ = createDepthBufferAttachment(reflectionWidth_, reflectionHeight_);
        unbindActiveFrameBuffer();
    }

    void initRefractionFrameBuffer() {
        refractionFb_ = createFrameBuffer();
        refractionTexture_ = createTextureAttachment(refractionWidth_, refractionHeight_);
        refractionDepthTexture_ = createDepthTextureAttachment(refractionWidth_, refractionHeight_);
        unbindActiveFrameBuffer();
    }

    void bindFrameBuffer(unsigned int frameBuffer, int width, int height) {
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
        glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glViewport(0, 0, width, height);
    }

    unsigned int createFrameBuffer() {
        unsigned int id;
        glGenFramebuffers(1, &id);
        glBindFramebuffer(GL_FRAMEBUFFER, id);
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
        return id;
    }

    unsigned int createTextureAttachment(int width, int height) {
        unsigned int textureId;
        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
                        GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, textureId, 0);

        return textureId;
    }

    unsigned int createDepthTextureAttachment(int width, int height) {
        unsigned int textureId;
        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height,
                    0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, textureId, 0);

        return textureId;
    }

    unsigned int createDepthBufferAttachment(int width, int height) {
        unsigned int depthBuf;
        glGenRenderbuffers(1, &depthBuf);
        glBindRenderbuffer(GL_RENDERBUFFER, depthBuf);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuf);

        return depthBuf;
    }
};
#endif