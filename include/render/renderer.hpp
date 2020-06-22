#ifndef RENDERER_H
#define RENDERER_H

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "drawable.hpp"
#include "game.hpp"
#include "global.hpp"
#include "shader.hpp"

/*
 * Forward declaration of classes from private headers
 */
class WaterFrameBuffer;
class GuiRenderer;
class VaoRenderer;
class TerrainRenderer;
class WaterRenderer;
class SkyRenderer;

typedef std::map<std::string, Shader *> ShaderMap;
typedef std::map<std::string, std::vector<Drawable *>> EntityMap;
typedef std::vector<Drawable *> DrawableList;

class Renderer {
  public:
    Renderer(std::vector<Shader *> shaders);
    ~Renderer();
    /*
     *
     */
    void render(DrawableList &lights, DrawableList &terrain, DrawableList &entities, DrawableList &sky,
                DrawableList &water, Game *game);
    /*
     *
     */
    void render(DrawableList &lights, DrawableList &terrain, DrawableList &entities, DrawableList &sky,
                DrawableList &water, DrawableList *gui, Game *game);

    void resolutionChange(int width, int height);

  private:
    ShaderMap shaderMap_;
    EntityMap entityMap_;
    int windowWidth_, windowHeight_ = 0;
    VaoRenderer *vaoRenderer_ = NULL;
    Shader *lightShader_ = NULL;
    SkyRenderer *skyRenderer_ = NULL;
    WaterFrameBuffer *waterFrameBuffer_ = NULL;
    GuiRenderer *guiRenderer_ = NULL;
    TerrainRenderer *terrainRenderer_ = NULL;
    WaterRenderer *waterRenderer_ = NULL;
    bool waterTypeQuality_ = false;
    /*
     *
     */
    void setupRenderer(std::vector<Shader *> shaders);
    /*
     * Puts entities in a hash map for batched drawing
     */
    void processEntities(std::vector<Drawable *> &entities);
    /*
     *
     */
    void renderScene(DrawableList &lights, DrawableList &terrain, DrawableList &sky, Game *game, glm::vec4 clipPlane);
    /*
     *
     */
    void renderList(Shader *shader, DrawableList &drawables, Game *game, glm::vec4 clipPlane);
    /*
     *
     */
    void renderEntities(Game *game, glm::vec4 clipPlane);
};
#endif