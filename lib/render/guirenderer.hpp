#ifndef GUIRENDERER_H
#define GUIRENDERER_H

#include "shader.hpp"
#include "vaorenderer.hpp"

class GuiRenderer {
  public:
    GuiRenderer(Shader *shader, VaoRenderer *vaoRenderer);
    void render(std::vector<Drawable *> *guiElements, Game *game);

  private:
    Shader *shader_;
    VaoRenderer *vaoRenderer_;
};
#endif