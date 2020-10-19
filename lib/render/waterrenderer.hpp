#ifndef WATERRENDERER_H
#define WATERRENDERER_H

#include <iostream>

#include "shader.hpp"
#include "vaorenderer.hpp"
#include "waterframebuffer.hpp"
#include "terraindatatypes.hpp"
/*
 * Water Renderer
 * TODO: Load dudv texture somewhere else and pass it to the renderer
 */
class WaterRenderer {
  public:
    WaterRenderer(VaoRenderer *vaoRenderer, int width, int height);
    ~WaterRenderer();
    void render(TerrainRenderDataVector &renderDataVector, SceneRenderData &sceneData);
    void addShader(Shader *shader);
    void bindReflectionFrameBuffer();
    void bindRefractionFrameBuffer();
    void unbindActiveFrameBuffer();
    void resolutionChange(int width, int height);

  private:
    Shader *shaderQ_ = nullptr;
    Shader *shaderP_ = nullptr;
    VaoRenderer *vaoRenderer_;
    WaterFrameBuffer *waterFrameBuffer_ = nullptr;
    unsigned int waterTexturePerformance_;
    unsigned int reflectionTexture_;
    unsigned int refractionTexture_;
    unsigned int dudvTexture_;
    unsigned int dudvTexturePerformance_;
    unsigned int normalTexturePerformance_;
    unsigned int normalTexture_;
    unsigned int depthTexture_;
    float moveFactor_ = 0.0;
    int windowWidth_, windowHeight_;

    void initPerformanceShader();
    void initQualityShader();
    void preparePerformance();
    void prepareQuality();
    void render_(TerrainObjectRenderData &renderData, SceneRenderData &sceneData);
};
#endif