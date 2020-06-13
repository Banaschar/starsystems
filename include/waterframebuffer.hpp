#ifndef WATERFRAMEBUFFER_H
#define WATERFRAMEBUFFER_H

class WaterFrameBuffer {
public:
    WaterFrameBuffer() {
        initReflectionFb();
        initRefractionFb();
    }

private:
    int reflectionFb;
    int reflectionTexture;
    int reflectionDepthBuffer;

    int refractionFb;
    int refractionTexture;
    int reflectionDepthBuffer;

    initRefractionFb() {

    }

    initReflectionFb() {
        
    }

    int createFb() {
        int fb = glGenFramebuffers();
        glBindFramebuffer(GL_FRAMEBUFFER, &fb);
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
        return fb;
    }

    int createTextureAttachment(int width, int height) {
        ;
    }
}
#endif