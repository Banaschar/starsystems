#ifndef SKYRENDERER_H
#define SKYRENDERER_H

#include "shader.hpp"
#include "vaorenderer.hpp"

class SkyRenderer {
  public:
    SkyRenderer(Shader *shader, VaoRenderer *vaoRenderer);
    void render(std::vector<Drawable *> skies, Game *game, glm::vec4 clipPlane);

  private:
    Shader *shader_;
    VaoRenderer *vaoRenderer_;
};
#endif