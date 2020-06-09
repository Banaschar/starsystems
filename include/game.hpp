#ifndef GAME_H
#define GAME_H

#include <glm/glm.hpp>
#include <vector>

#include "drawable.hpp"
#include "view.hpp"

class Game {
public:
    Game(View view) : view_(view) {
        ;
    }

    void addModel(Drawable *model) {
        drawables_.push_back(model);
    }

    void addLight(Drawable *model) {
        lightSources_.push_back(model);
    }

    void addSkyBox(Drawable *model) {
        skybox_ = model;
    }

    View& getView() {
        return view_;
    }

    std::vector<Drawable*>& getLightSources() {
        return lightSources_;
    }

    std::vector<Drawable*>& getDrawables() {
        return drawables_;
    }

    Drawable* getLightSource() {
        return lightSources_[0];
    }

    Drawable* getSkyBox() {
        return skybox_;
    }

private:
    View view_;
    Drawable *skybox_ = NULL;
    std::vector<Drawable*> lightSources_;
    std::vector<Drawable*> drawables_;
};
#endif