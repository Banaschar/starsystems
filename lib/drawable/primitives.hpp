#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#include "mesh.hpp"
#include <vector>

class Primitives {
  public:
    static Mesh *createPlane(int dimension);

    static Mesh *createQuad();

    static Mesh *createQuad2d();

    static Mesh *createCube(int side);

  private:
    static std::vector<glm::vec3> calculateVertexNormalAverages(std::vector<glm::vec3> &pos,
                                                                std::vector<unsigned int> &indices);
};
#endif