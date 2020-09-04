#ifndef DRAWABLEFACTORY_H
#define DRAWABLEFACTORY_H

#include "drawable.hpp"
#include "light.hpp"

class TerrainTile;
enum class PrimitiveType { QUAD, QUAD2D, PLANE, CUBE };

class DrawableFactory {
  public:
    static Drawable *createPrimitive(PrimitiveType prim, ShaderType type, int size = 1);
    static Drawable *createTexturedPrimitive(PrimitiveType primType, ShaderType type, Texture texture);
    static Drawable *createCubeMap(std::vector<std::string> cubeTexPaths, ShaderType type);
    static Drawable *createModel(const std::string &path, ShaderType type);
    static Drawable *createLight(const std::string &path, ShaderType type);
    static TerrainTile *createWaterTile(glm::vec3 position, int scale, glm::vec3 color);
    static Drawable *createLight(PrimitiveType prim, ShaderType type, int size = 1);

  private:
    static Mesh *createPrimitiveMesh(PrimitiveType prim, int size = 1);
};
#endif