#include <iostream>

#include "scene.hpp"

Scene::Scene(Game *game) : game_(game), lightSources_(game_->getLightSources()),
                            models_(game_->getDrawables()), skybox_(game_->getSkyBox()) {
    autoRotate_ = false;
}

Scene::~Scene() {
    delete game_;
}

void Scene::update() {
    game_->getView().update();
}

void Scene::render() {
    for (int i = 0; i < lightSources_.size(); i++) {
        lightSources_[i]->update(game_);
        lightSources_[i]->draw(game_);
    }

    for (int i = 0; i < models_.size(); i++) {
        models_[i]->update(game_);
        models_[i]->draw(game_);
    }

    if (skybox_ != NULL) {
        skybox_->update(game_);
        skybox_->draw(game_);
    }
}

void Scene::setAutoRotate(bool value) {
    game_->getView().setAutoRotate(value);
}