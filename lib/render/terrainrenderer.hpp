#ifndef TERRAINRENDERER_H
#define TERRAINRENDERER_H

#include "shader.hpp"
#include "terrain.hpp"
#include "vaorenderer.hpp"

class TerrainRenderer {
  public:
    TerrainRenderer(Shader *shader, VaoRenderer *vaoRenderer);

    void setupTextures();

    void bindTextures();

    void render(std::vector<Drawable *> terrains, Game *game, glm::vec4 clipPlane);

  private:
    Shader *shader_;
    VaoRenderer *vaoRenderer_;
    std::vector<Texture> textures_;
};
#endif