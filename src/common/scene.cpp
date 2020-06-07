#include <iostream>

#include "scene.hpp"

Scene::Scene(View view, Model light) : view_(view), lightSource_(light) {
    autoRotate_ = false;
}

void Scene::addModel(Drawable *model) {
    models_.push_back(model);
}

void Scene::update() {
    if (autoRotate_)
        view_.rotateCamera();
    //view_.updateFromInputs();
}

void Scene::render() {
    lightSource_.update(view_);
    lightSource_.draw(view_, lightSource_.getPosition());
    for (int i = 0; i < models_.size(); i++) {
        models_[i]->update(view_);
        models_[i]->draw(view_, lightSource_.getPosition());
    }
}

void Scene::setAutoRotate(bool value) {
    autoRotate_ = value;
}