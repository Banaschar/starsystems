#ifndef WATERFRAMEBUFFER_H
#define WATERFRAMEBUFFER_H

#define REFLECTION_WIDTH 320
#define REFLECTION_HEIGHT 180
#define REFRACTION_WIDTH 1280
#define REFRACTION_HEIGHT 720

class WaterFrameBuffer {
  public:
    WaterFrameBuffer(int windowWitdh, int windowHeight);
    ~WaterFrameBuffer();

    unsigned int getReflectionTexture();

    unsigned int getRefractionTexture();

    unsigned int getRefractionDepthTexture();

    void unbindActiveFrameBuffer();

    void bindReflectionFrameBuffer();

    void bindRefractionFrameBuffer();

    void resolutionChange(int width, int height);

  private:
    unsigned int reflectionFb_;
    unsigned int reflectionTexture_;
    unsigned int reflectionDepthBuffer_;

    unsigned int refractionFb_;
    unsigned int refractionTexture_;
    unsigned int refractionDepthTexture_;
    int reflectionWidth_, reflectionHeight_, refractionHeight_, refractionWidth_;
    int windowWitdh_, windowHeight_;

    void initReflectionFrameBuffer();

    void initRefractionFrameBuffer();

    void bindFrameBuffer(unsigned int frameBuffer, int width, int height);

    unsigned int createFrameBuffer();

    unsigned int createTextureAttachment(int width, int height);

    unsigned int createDepthTextureAttachment(int width, int height);

    unsigned int createDepthBufferAttachment(int width, int height);
};
#endif