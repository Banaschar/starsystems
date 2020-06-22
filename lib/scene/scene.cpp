#include <iostream>

#include "scene.hpp"

Scene::Scene(Game *game, Renderer *renderer)
    : game_(game), renderer_(renderer), lights_(game_->getLights()), terrain_(game_->getTerrain()),
      entities_(game_->getEntities()), sky_(game_->getSky()), water_(game_->getWater()) {
    autoRotate_ = false;
    setupScene();
}

Scene::~Scene() {
    delete game_;
    delete renderer_;
    if (gui_)
        delete gui_;
}

void Scene::addGui(Gui *gui) {
    gui_ = gui;
    /*
    if (waterFrameBuffer_) {
        gui_->addGuiElement(waterFrameBuffer_->getReflectionTexture(), glm::vec2(0.0,0.0), glm::vec2(320,180));
        gui_->addGuiElement(waterFrameBuffer_->getRefractionTexture(), glm::vec2(0.0,2.0), glm::vec2(320,180));
    }
    */
}

void Scene::setupScene() {
    int width, height;
    game_->getView().getWindowSize(&width, &height);
    renderer_->resolutionChange(width, height);
}

/*
 * Update game object, holding player, with terrain for collision?
 */
void Scene::update() {
    game_->getView().update();
    if (gui_)
        gui_->update(game_);
}

/*
 * TODO:
 * Each drawable needs a unique identifier, so it can be deleted from
 */
void Scene::render() {
    if (gui_)
        renderer_->render(lights_, terrain_, entities_, sky_, water_, gui_->getGuiElements(), game_);
    else
        renderer_->render(lights_, terrain_, entities_, sky_, water_, game_);
}

void Scene::setAutoRotate(bool value) {
    game_->getView().setAutoRotate(value);
}