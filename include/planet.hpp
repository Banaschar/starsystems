#ifndef PLANET_H
#define PLANET_H

#include <glm/glm.hpp>
#include <string>

#include "drawable.hpp"
#include "global.hpp"

const std::string DEFAULT_TYPE = "planet"

class Planet : public Drawable {
public:
    Planet(Mesh mesh, float orbitSpeed, std::string type = DEFAULT_TYPE);

    void update(Game *game) override;
private:
    float orbitSpeed_;
    float orbitRadius_;
    float rotationVal_;
};
#endif