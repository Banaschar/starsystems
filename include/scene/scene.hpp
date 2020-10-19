#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include "scenerenderdata.hpp"

class Drawable;
class Gui;
class Renderer;
class View;
class TerrainManager;

class Scene {
  public:
    Scene(Renderer *renderer, View *view);
    Scene(Renderer *renderer, View *view, TerrainManager *terrainManager);
    ~Scene();
    void update();
    void render();
    
    void addEntity(Drawable *entity);
    void addLight(Drawable *light);
    void addSun(Drawable *sun);
    void addSky(Drawable *sky);

    Gui *getGui();
    View *getView();
    Drawable *getSun();
    TerrainManager *getTerrainManager();

  private:
    void setupScene();
    SceneRenderData sceneRenderData_;
    Renderer *renderer_;
    View *view_;
    TerrainManager *terrainManager_;
    Gui *gui_;
    std::vector<Drawable *> lightsRenderList_;
    std::vector<Drawable *> entities_;
    std::vector<Drawable *> sky_;
};
#endif