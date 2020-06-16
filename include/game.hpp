#ifndef GAME_H
#define GAME_H

#include <glm/glm.hpp>
#include <vector>
#include <iostream>

#include "drawable.hpp"
#include "view.hpp"
#include "light.hpp"

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
        for (Drawable *m : terrain_) {
            delete m;
        }
        for (Drawable *m : entities_) {
            delete m;
        }
        for (Drawable *m : lights_) {
            delete m;
        }
        for (Drawable *m : water_) {
            delete m;
        }
        for (Drawable *m : sky_) {
            delete m;
        }
    }

    void addEntity(Drawable *entity) {
        entities_.push_back(entity);
    }

    void addLight(Drawable *light) {
        Light* tmp = dynamic_cast<Light*>(light);
        if (tmp)
            lights_.push_back(light);
        else
            std::cout << "Model added as light is not actually of class Light" << std::endl;
    }

    void addSun(Drawable *sun) {
        Light* tmp = dynamic_cast<Light*>(sun);
        if (tmp) {
            sun_ = sun;
            if (tmp->hasModel())
                lights_.push_back(sun);
        } else {
            std::cout << "Model added as sun is not actually of class Light" << std::endl;
        }
    }

    void addSky(Drawable *sky) {
        sky_.push_back(sky);
    }

    void addTerrain(Drawable *terrain) {
        terrain_.push_back(terrain);
    }

    void addWater(Drawable *water) {
        water_.push_back(water);
    }

    View& getView() {
        return view_;
    }

    std::vector<Drawable*>& getLights() {
        return lights_;
    }

    std::vector<Drawable*>& getEntities() {
        return entities_;
    }

    std::vector<Drawable*>& getTerrain() {
        return terrain_;
    }

    std::vector<Drawable*>& getWater() {
        return water_;
    }

    Drawable* getSun() {
        if (sun_)
            return sun_;
        else {
            std::cout << "Requested non-existing sun, this returns NULL and therefore probably gives you a SegFault" << std::endl;
            return NULL;
        }
    }

    Drawable* getLightSource(int index = 0) {
        return lights_.at(index);
    }

    std::vector<Drawable*>& getSky() {
        return sky_;
    }

private:
    View view_;
    Drawable *sun_ = NULL;
    std::vector<Drawable*> sky_;
    std::vector<Drawable*> lights_;
    std::vector<Drawable*> terrain_;
    std::vector<Drawable*> entities_;
    std::vector<Drawable*> water_;
};
#endif