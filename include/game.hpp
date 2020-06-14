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
        lights_.push_back(light);
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

    Drawable* getLightSource() {
        return lights_[0];
    }

    std::vector<Drawable*>& getSky() {
        return sky_;
    }

private:
    View view_;
    std::vector<Drawable*> sky_;
    std::vector<Drawable*> lights_;
    std::vector<Drawable*> terrain_;
    std::vector<Drawable*> entities_;
    std::vector<Drawable*> water_;
};
#endif