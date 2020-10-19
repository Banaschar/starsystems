#ifndef TERRAINRENDERER_H
#define TERRAINRENDERER_H

#include "shader.hpp"
#include "terraintile.hpp"
#include "vaorenderer.hpp"
#include "terraindatatypes.hpp"

class TerrainRenderer {
  public:
    TerrainRenderer(Shader *shader, VaoRenderer *vaoRenderer);
    void setupTextures();
    void render(TerrainRenderDataVector &terrainRenderDataVector, SceneRenderData &sceneData);

  private:
    Shader *shader_;
    VaoRenderer *vaoRenderer_;
    std::vector<Texture> textures_;
    void render_(TerrainObjectRenderData &renderData, SceneRenderData &sceneData);
    void bindTextures();
    void bindTextureList(TextureList &textureList);
};
#endif