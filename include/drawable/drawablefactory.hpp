#ifndef DRAWABLEFACTORY_H
#define DRAWABLEFACTORY_H

#include "drawable.hpp"
#include "light.hpp"

enum class PrimitiveType { QUAD, QUAD2D, PLANE, CUBE };

class DrawableFactory {
  public:
    static Drawable *createPrimitive(PrimitiveType prim, std::string type, int size = 1);
    static Drawable *createTexturedPrimitive(PrimitiveType primType, std::string type, Texture texture);
    static Drawable *createCubeMap(std::vector<std::string> cubeTexPaths, std::string type);
    static Drawable *createModel(const std::string &path, const std::string &type);
    static Drawable *createLight(const std::string &path, const std::string &type);
    static Drawable *createWaterTile(glm::vec3 position, int scale, glm::vec3 color);
    static Drawable *createLight(PrimitiveType prim, std::string type, int size = 1);

  private:
    static bool createPrimitiveMesh(PrimitiveType prim, Mesh *mesh, int size = 1);
};
#endif