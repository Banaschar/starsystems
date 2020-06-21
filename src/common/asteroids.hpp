#ifndef ASTEROIDS_H
#define ASTEROIDS_H

#include <glm/glm.hpp>

#include "model.hpp"
#include "game.hpp"
#include "drawable.hpp"
#include "global.hpp"

class Asteroids : public Drawable {
public:
    Planet(int num) : num_(num) {}
    void draw(Game *game) {
        model_.draw(game);
    }
    void update(Game *game) {
        model_.update();
    }
    glm::vec3 getPosition(int index = 0) {
        return mode_.getPosition(index);
    }
private:
    Model model_;
    float orbitSpeed_;
    float orbitRadius_;
    float rotationVal_;
};
#endif