#ifndef DRAWABLE_H
#define DRAWABLE_H

#include <glm/glm.hpp>

#include "view.hpp"
#include "shader.hpp"

class Drawable {
    public:
        virtual void draw(Shader shader) = 0;
};
#endif