#ifndef DRAWABLE_H
#define DRAWABLE_H

#include <glm/glm.hpp>

class Game;

class Drawable {
    public:
        virtual void draw(Game &game) = 0;
        virtual void update(Game &game) = 0;
        virtual glm::vec3 getPosition() = 0;
};
#endif