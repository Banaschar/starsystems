#ifndef PLANET_H
#define PLANET_H

#include <glm/glm.hpp>

#include "model.hpp"
#include "game.hpp"
#include "drawable.hpp"
#include "global.hpp"

class Planet : public Drawable {
public:
    Planet(Model model, float orbitSpeed);
    void draw(Game &game);
    void update(Game &game);
    glm::vec3 getPosition();
private:
    Model model_;
    float orbitSpeed_;
    float orbitRadius_;
    float rotationVal_;
};
#endif