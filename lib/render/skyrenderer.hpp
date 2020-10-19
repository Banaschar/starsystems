#ifndef SKYRENDERER_H
#define SKYRENDERER_H

#include "shader.hpp"
#include "vaorenderer.hpp"
#include "scenerenderdata.hpp"

class SkyRenderer {
  public:
    SkyRenderer(Shader *shader, VaoRenderer *vaoRenderer);
    void render(std::vector<Drawable *> skies, SceneRenderData &sceneData);

  private:
    Shader *shader_;
    VaoRenderer *vaoRenderer_;
};
#endif