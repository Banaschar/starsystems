#include <iostream>

#include "scene.hpp"
#include "drawable.hpp"
#include "renderer.hpp"
#include "view.hpp"
#include "terrainmanager.hpp"
#include "gui.hpp"

Scene::Scene(Renderer *renderer, View *view) : renderer_(renderer), view_(view) {
    terrainManager_ = new TerrainManager();
    setupScene();
}

Scene::Scene(Renderer *renderer, View *view, TerrainManager *terrainManager) : renderer_(renderer), view_(view), terrainManager_(terrainManager) {
    setupScene();
}

Scene::~Scene() {
    delete renderer_;
    delete terrainManager_;
    delete gui_;

    for (Drawable *d : sceneRenderData_.sceneLightList) {
        Light *tmp = dynamic_cast<Light *>(d);
        delete tmp;
    }
}

void Scene::setupScene() {
    gui_ = new Gui();
    sceneRenderData_.view = view_;
}

void Scene::update() {
    view_->update();
    if (gui_)
        gui_->update();
    terrainManager_->update(view_);
}

void Scene::render() {
    renderer_->render(terrainManager_->getTerrainRenderData(), lightsRenderList_, entities_, sky_, gui_ ? gui_->getGuiElements() : NULL, sceneRenderData_);
}

void Scene::addEntity(Drawable *entity) {
    if (entity)
        entities_.push_back(entity);
}

void Scene::addLight(Drawable *light) {
    Light *tmp = dynamic_cast<Light *>(light);
    if (tmp) {
        sceneRenderData_.sceneLightList.push_back(light);
        if (tmp->hasModel())
            lightsRenderList_.push_back(light);
    }
    else
        fprintf(stdout, "Model added as light is not actually of class Light\n");
}

void Scene::addSun(Drawable *sun) {
    Light *tmp = dynamic_cast<Light *>(sun);
    if (tmp) {
        sceneRenderData_.sun = sun;
        sceneRenderData_.sceneLightList.push_back(sun);
        if (tmp->hasModel())
            lightsRenderList_.push_back(sun);
    } else {
        fprintf(stdout, "Model added as sun is not actually of class Light\n");
    }
}

void Scene::addSky(Drawable *sky) {
    sky_.push_back(sky);
}

Drawable *Scene::getSun() {
    return sceneRenderData_.sun;
}

Gui *Scene::getGui() {
    return gui_;
}

View *Scene::getView() {
    return view_;
}

TerrainManager *Scene::getTerrainManager() {
    return terrainManager_;
}