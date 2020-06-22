#ifndef WATERRENDERER_H
#define WATERRENDERER_H

#include <iostream>

#include "shader.hpp"
#include "vaorenderer.hpp"

/*
 * Water Renderer
 * TODO: Load dudv texture somewhere else and pass it to the renderer
 */
class WaterRenderer {
  public:
    WaterRenderer(Shader *shader, VaoRenderer *vaoRenderer, unsigned int reflectionTexture,
                  unsigned int refractionTexture, unsigned int depthTexture);
    WaterRenderer(Shader *shader, VaoRenderer *vaoRenderer);
    void render(std::vector<Drawable *> water, Game *game);

  private:
    Shader *shader_;
    VaoRenderer *vaoRenderer_;
    unsigned int waterTextureLow_;
    unsigned int reflectionTexture_;
    unsigned int refractionTexture_;
    unsigned int dudvTexture_;
    unsigned int normalTexture_;
    unsigned int depthTexture_;
    float moveFactor_ = 0.0;
    bool renderPerformance_;

    void preparePerformance();

    void prepareQuality();
};
#endif