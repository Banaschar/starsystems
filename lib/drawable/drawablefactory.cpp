#include "drawablefactory.hpp"

#include <cstdio>

#include "assetloader.hpp"
#include "primitives.hpp"
#include "textureloader.hpp"
#include "terraintile.hpp"

Mesh *DrawableFactory::createPrimitiveMesh(PrimitiveType prim, int size) {
    switch (prim) {
    case PrimitiveType::QUAD:
        return Primitives::createQuad();
        break;
    case PrimitiveType::QUAD2D:
        return Primitives::createQuad2d();
        break;
    case PrimitiveType::PLANE:
        return Primitives::createPlane(size);
        break;
    case PrimitiveType::CUBE:
        return Primitives::createCube(size);
        break;
    default:
        fprintf(stdout, "Invalid primitive type\n");
        return NULL;
    }

    return NULL;
}

Drawable *DrawableFactory::createPrimitive(PrimitiveType prim, ShaderType type, int size) {
    Mesh *mesh;
    if (!(mesh = createPrimitiveMesh(prim, size)))
        return NULL;
    else
        return new Drawable(mesh, type);
}

Drawable *DrawableFactory::createTexturedPrimitive(PrimitiveType primType, ShaderType type, Texture texture) {
    Drawable *tmp;
    if (!(tmp = createPrimitive(primType, type)))
        return NULL;

    tmp->addTexture(texture);
    return tmp;
}

Drawable *DrawableFactory::createCubeMap(std::vector<std::string> cubeTexPaths, ShaderType type) {
    unsigned int texId = TextureLoader::loadCubeMap(cubeTexPaths);
    Texture cubemapTexture;
    cubemapTexture.id = texId;
    cubemapTexture.type = "texture_cubemap";
    return createTexturedPrimitive(PrimitiveType::CUBE, type, cubemapTexture);
}

Drawable *DrawableFactory::createModel(const std::string &path, ShaderType type) {
    std::vector<Mesh*> meshes;
    if (!AssetLoader::loadModel(path, &meshes))
        return NULL;
    else
        return new Drawable(meshes, type);
}

TerrainTile *DrawableFactory::createWaterTile(glm::vec3 position, int scale, glm::vec3 color) {
    Mesh *mesh;
    if (!(mesh = createPrimitiveMesh(PrimitiveType::QUAD)))
        return NULL;
    TerrainTile *tmp = new TerrainTile(mesh);
    glm::vec3 scaleVec = glm::vec3(scale, 1, scale);
    tmp->transform(&scaleVec, &position, NULL);

    return tmp;
}

Drawable *DrawableFactory::createLight(const std::string &path, ShaderType type) {
    std::vector<Mesh*> meshes;
    if (!AssetLoader::loadModel(path, &meshes))
        return NULL;
    else
        return new Light(meshes, type);
}

Drawable *DrawableFactory::createLight(PrimitiveType prim, ShaderType type, int size) {
    Mesh *mesh;
    if (!(mesh = createPrimitiveMesh(prim)))
        return NULL;
    else
        return new Light(mesh, type);
}