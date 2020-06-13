#ifndef GAME_H
#define GAME_H

#include <glm/glm.hpp>
#include <vector>

#include "drawable.hpp"
#include "view.hpp"

class Game {
public:
    Game(View view) : view_(view) {
        /* TODO: rethink this
         * This is only here because I copy the view object in here,
         * and therefore the callback for glfw points to the wrong view object.
         * Could be solved by just getting a pointer to view instead of copying the 
         * object. Would also be more harmonic, as the all passed around objects would need
         * the "->" accessor. 
         */
        view_.setupInput();
    }
    ~Game() {
        for (Drawable *m : drawables_) {
            delete m;
        }
        for (Drawable *m : lightSources_) {
            delete m;
        }

        for (Drawable *m : water_) {
            delete m;
        }

        delete skybox_;
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

    void addWater(Drawable *water) {
        water_.push_back(water);
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

    std::vector<Drawable*>& getWater() {
        return water_;
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
    std::vector<Drawable*> water_;
    std::vector<Drawable*> drawables_;
};
#endif