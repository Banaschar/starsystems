#include "waterframebuffer.hpp"
#include "oglheader.hpp"

WaterFrameBuffer::WaterFrameBuffer(int windowWitdh, int windowHeight)
    : windowWitdh_(windowWitdh), windowHeight_(windowHeight) {
    reflectionWidth_ = REFLECTION_WIDTH;
    reflectionHeight_ = REFLECTION_HEIGHT;
    refractionWidth_ = REFRACTION_WIDTH;
    refractionHeight_ = REFRACTION_HEIGHT;
    initReflectionFrameBuffer();
    initRefractionFrameBuffer();
}

WaterFrameBuffer::~WaterFrameBuffer() {
    glDeleteFramebuffers(1, &reflectionFb_);
    glDeleteTextures(1, &reflectionTexture_);
    glDeleteRenderbuffers(1, &reflectionDepthBuffer_);
    glDeleteFramebuffers(1, &refractionFb_);
    glDeleteTextures(1, &refractionTexture_);
    glDeleteTextures(1, &refractionDepthTexture_);
}

unsigned int WaterFrameBuffer::getReflectionTexture() {
    return reflectionTexture_;
}

unsigned int WaterFrameBuffer::getRefractionTexture() {
    return refractionTexture_;
}

unsigned int WaterFrameBuffer::getRefractionDepthTexture() {
    return refractionDepthTexture_;
}

void WaterFrameBuffer::resolutionChange(int width, int height) {
    windowWitdh_ = width;
    windowHeight_ = height;
}

void WaterFrameBuffer::unbindActiveFrameBuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, windowWitdh_, windowHeight_);
}

void WaterFrameBuffer::bindReflectionFrameBuffer() {
    bindFrameBuffer(reflectionFb_, reflectionWidth_, reflectionHeight_);
}

void WaterFrameBuffer::bindRefractionFrameBuffer() {
    bindFrameBuffer(refractionFb_, refractionWidth_, refractionHeight_);
}

void WaterFrameBuffer::initReflectionFrameBuffer() {
    reflectionFb_ = createFrameBuffer();
    reflectionTexture_ = createTextureAttachment(reflectionWidth_, reflectionHeight_);
    reflectionDepthBuffer_ = createDepthBufferAttachment(reflectionWidth_, reflectionHeight_);
    unbindActiveFrameBuffer();
}

void WaterFrameBuffer::initRefractionFrameBuffer() {
    refractionFb_ = createFrameBuffer();
    refractionTexture_ = createTextureAttachment(refractionWidth_, refractionHeight_);
    refractionDepthTexture_ = createDepthTextureAttachment(refractionWidth_, refractionHeight_);
    unbindActiveFrameBuffer();
}

void WaterFrameBuffer::bindFrameBuffer(unsigned int frameBuffer, int width, int height) {
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    //glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, width, height);
}

unsigned int WaterFrameBuffer::createFrameBuffer() {
    unsigned int id;
    glGenFramebuffers(1, &id);
    glBindFramebuffer(GL_FRAMEBUFFER, id);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    return id;
}

unsigned int WaterFrameBuffer::createTextureAttachment(int width, int height) {
    unsigned int textureId;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, textureId, 0);

    return textureId;
}

unsigned int WaterFrameBuffer::createDepthTextureAttachment(int width, int height) {
    unsigned int textureId;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, textureId, 0);

    return textureId;
}

unsigned int WaterFrameBuffer::createDepthBufferAttachment(int width, int height) {
    unsigned int depthBuf;
    glGenRenderbuffers(1, &depthBuf);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuf);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuf);

    return depthBuf;
}