#ifndef PLANET_H
#define PLANET_H

#include <glm/glm.hpp>

#include "model.hpp"
#include "view.hpp"
#include "drawable.hpp"
#include "global.hpp"

class Planet : public Drawable {
public:
    Planet(Model model, float orbitSpeed);
    void draw(View view, glm::vec3 lightPos);
    void update(View view);
private:
    Model model_;
    float orbitSpeed_;
    float orbitRadius_;
    float rotationVal_;
};
#endif