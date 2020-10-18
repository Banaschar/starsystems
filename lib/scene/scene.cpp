#include <iostream>

#include "scene.hpp"
#include "drawable.hpp"
#include "renderer.hpp"
#include "view.hpp"
#include "terrainmanager.hpp"

Scene::Scene(Renderer *renderer, View *view) : renderer_(renderer), view_(view) {
    terrainManager_ = new TerrainManager();
    setupScene();
}

Scene::Scene(Renderer *renderer, View *view, TerrainManager *terrainmanager) : renderer_(renderer), view_(view), terrainManager_(terrainManager) {
    setupScene();
}

Scene::~Scene() {
    delete game_;
    delete renderer_;
    delete terrainManager_;
    delete gui_;
}

void Scene::setupScene() {
    gui_ = new Gui();
    sceneRenderData_.view = view_;
}

void Scene::update() {
    game_->update();
    if (gui_)
        gui_->update(game_);
    terrainManager_->update(game_->getView());
}

void Scene::render() {
    renderer_->render(terrainManager_->getTerrainRenderData(), lights_, entities_, sky_, gui_ ? gui_->getGuiElements() : NULL, sceneRenderData_);
}

void Scene::addEntity(Drawable *entity) {
    if (entity)
        entities_.push_back(entity);
}

void Scene::addLight(Drawable *light) {
    Light *tmp = dynamic_cast<Light *>(light);
    if (tmp)
        lights_.push_back(light);
    else
        fprintf(stdout, "Model added as light is not actually of class Light\n");
}

void Scene::addSun(Drawable *sun) {
    Light *tmp = dynamic_cast<Light *>(sun);
    if (tmp) {
        sun_ = sun;
        if (tmp->hasModel())
            lights_.push_back(sun);
    } else {
        fprintf(stdout, "Model added as sun is not actually of class Light\n");
    }
}

void Scene::addSky(Drawable *sky) {
    sky_.push_back(sky);
}

Drawable *getSun() {
    return sceneRenderData.sun;
}

Gui *getGui() {
    return gui_;
}

View *getView() {
    return view_;
}