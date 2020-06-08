#include <iostream>

#include "scene.hpp"

Scene::Scene(Game game) : game_(game), lightSources_(game_.getLightSources()),
                            models_(game_.getDrawables()) {
    autoRotate_ = false;
}

Scene::~Scene() {
    for (Drawable *m : models_) {
        delete m;
    }
    for (Drawable *m : lightSources_) {
        delete m;
    }
}

void Scene::update() {
    if (autoRotate_)
        game_.getView().rotateCamera();
    //view_.updateFromInputs();
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
}

void Scene::setAutoRotate(bool value) {
    autoRotate_ = value;
}