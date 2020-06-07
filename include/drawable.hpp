#ifndef DRAWABLE_H
#define DRAWABLE_H

#include <glm/glm.hpp>
#include "view.hpp"

class Drawable {
    public:
        virtual void draw(View view, glm::vec3 lightPos) = 0;
        virtual void update(View view) = 0;
};
#endif