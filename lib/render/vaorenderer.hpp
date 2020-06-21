#ifndef VAORENDERER_H
#define VAORENDERER_H

#include "mesh.hpp"

class VaoRenderer {
public:
    VaoRenderer();

    void draw(Mesh &mesh);
};
#endif