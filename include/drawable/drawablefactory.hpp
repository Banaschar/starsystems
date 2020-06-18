#ifndef DRAWABLEFACTORY_H
#define DRAWABLEFACTORY_H

#include "drawable.hpp"
#include "primitives.hpp"
#include "assetloader.hpp"
#include "planet.hpp"

enum class PrimitiveType {
    QUAD,
    QUAD2D,
    PLANE,
    CUBE
};

class DrawableFactory {
public:
    static Drawable* createPrimitive(PrimitiveType prim, std::string type, int size = 1) {
        switch(prim) {
            case PrimitiveType::QUAD:
                return new Drawable(Primitves.createQuad(), type);
                break;
            case PrimitiveType::QUAD2D:
                return new Drawable(Primitves.createQuad2d(), type);
                break;
            case PrimitiveType::PLANE:
                return new Drawable(Primitves.createPlane(size), type);
                break;
            case PrimitiveType::CUBE:
                return new Drawable(Primitves.createCube(size), type);
                break;
            default:
                std::cout << "Invalid primitive type" << std::endl;
                return NULL;
        }
    }
    static Drawable* createModel(const std::string &path, std::string type) {
        return new Drawable(AssetLoader::loadModel(path), type);
    }

    static Drawable* createPlanet(const std::string &path, float orbitSpeed, std::string type) {
        return new Planet(AssetLoader::loadModel(path), orbitSpeed, type);
    }
}
#endif