#include <iostream>

#include "scene.hpp"

Scene::Scene(Game *game, Renderer *renderer) : 
                            game_(game), renderer_(renderer), lights_(game_->getLights()),
                            terrain_(game_->getTerrain()), entities_(game_->getEntities()), 
                            sky_(game_->getSky()), water_(game_->getWater()) {
    autoRotate_ = false;
    setupScene();
}

Scene::~Scene() {
    delete game_;
    delete renderer_;
}

void Scene::setupScene() {
    /*
    int width, height;
    game_->getView().getWindowSize(&width, &height);
    renderer_->setReflectiveWaterRender(width, height);
    */
}

/*
 * Update game object, holding player, with terrain for collision?
 */
void Scene::update() {
    game_->getView().update();
}

/*
 * TODO:
 * Each drawable needs a unique identifier, so it can be deleted from
 */
void Scene::render() {

    renderer_->render(lights_, terrain_, entities_, sky_, water_, game_);
}

void Scene::setAutoRotate(bool value) {
    game_->getView().setAutoRotate(value);
}