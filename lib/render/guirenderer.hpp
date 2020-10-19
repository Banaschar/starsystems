#ifndef GUIRENDERER_H
#define GUIRENDERER_H

#include "shader.hpp"
#include "vaorenderer.hpp"
#include "scenerenderdata.hpp"

class GuiRenderer {
  public:
    GuiRenderer(Shader *shader, VaoRenderer *vaoRenderer);
    void render(std::vector<Drawable *> *guiElements, SceneRenderData &sceneData);

  private:
    Shader *shader_;
    VaoRenderer *vaoRenderer_;
};
#endif