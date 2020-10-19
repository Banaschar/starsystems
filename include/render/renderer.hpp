#ifndef RENDERER_H
#define RENDERER_H

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "drawable.hpp"
#include "global.hpp"
#include "shader.hpp"
#include "terraindatatypes.hpp"

/*
 * Forward declaration of classes from private headers
 */
class GuiRenderer;
class VaoRenderer;
class TerrainRenderer;
class WaterRenderer;
class SkyRenderer;
class PostProcessor;

typedef std::map<ShaderType, Shader *> ShaderMap;
typedef std::map<ShaderType, std::vector<Drawable *>> EntityMap;
typedef std::vector<Drawable *> DrawableList;

class Renderer {
  public:
    Renderer(std::vector<Shader *> shaders, int winWidth, int winHeight);
    ~Renderer();
    /*
     *
     */
    void render(TerrainRenderDataVector &terrainRenderData, DrawableList &lights, DrawableList &entities, DrawableList &sky,
                DrawableList *gui, SceneRenderData &sceneRenderData);

    void resolutionChange(int width, int height);

    unsigned int DEBUG_getPostProcessingTexture();

    void setPolygonRenderModeWireFrame(bool set);

  private:
    ShaderMap shaderMap_;
    EntityMap entityMap_;
    int windowWidth_, windowHeight_ = 0;
    VaoRenderer *vaoRenderer_ = NULL;
    Shader *lightShader_ = NULL;
    SkyRenderer *skyRenderer_ = NULL;
    GuiRenderer *guiRenderer_ = NULL;
    TerrainRenderer *terrainRenderer_ = NULL;
    WaterRenderer *waterRenderer_ = NULL;
    PostProcessor *postProcessorAtmosphere_ = NULL;
    bool waterTypeQuality_ = false;
    Shader *debugShader_ = nullptr;
    /*
     *
     */
    void setupRenderer(std::vector<Shader *> shaders);
    /*
     * Puts entities in a hash map for batched drawing
     */
    void processEntities(DrawableList &entities);
    /*
     *
     */
    void renderScene(TerrainRenderDataVector &terrainRenderData, DrawableList &lights, DrawableList &sky, SceneRenderData &sceneData);
    /*
     *
     */
    void renderList(Shader *shader, DrawableList &drawables, SceneRenderData &sceneData);
    /*
     *
     */
    void renderEntities(SceneRenderData &sceneData);
    /*
     *
     */
    void renderDebug(TerrainRenderDataVector &terrainRenderData, SceneRenderData &sceneData);
};
#endif