#include "drawablefactory.hpp"

#include <cstdio>

#include "assetloader.hpp"
#include "primitives.hpp"
#include "textureloader.hpp"

bool DrawableFactory::createPrimitiveMesh(PrimitiveType prim, Mesh *mesh, int size) {
    switch (prim) {
    case PrimitiveType::QUAD:
        *mesh = Primitives::createQuad();
        break;
    case PrimitiveType::QUAD2D:
        *mesh = Primitives::createQuad2d();
        break;
    case PrimitiveType::PLANE:
        *mesh = Primitives::createPlane(size);
        break;
    case PrimitiveType::CUBE:
        *mesh = Primitives::createCube(size);
        break;
    default:
        fprintf(stdout, "Invalid primitive type\n");
        return false;
    }

    return true;
}

Drawable *DrawableFactory::createPrimitive(PrimitiveType prim, std::string type, int size) {
    Mesh mesh;
    if (!createPrimitiveMesh(prim, &mesh))
        return NULL;
    else
        return new Drawable(mesh, type);
}

Drawable *DrawableFactory::createTexturedPrimitive(PrimitiveType primType, std::string type, Texture texture) {
    Drawable *tmp;
    if (!(tmp = createPrimitive(primType, type)))
        return NULL;

    tmp->addTexture(texture);
    return tmp;
}

Drawable *DrawableFactory::createCubeMap(std::vector<std::string> cubeTexPaths, std::string type) {
    unsigned int texId = TextureLoader::loadCubeMap(cubeTexPaths);
    Texture cubemapTexture;
    cubemapTexture.id = texId;
    cubemapTexture.type = "texture_cubemap";
    return createTexturedPrimitive(PrimitiveType::CUBE, type, cubemapTexture);
}

Drawable *DrawableFactory::createModel(const std::string &path, const std::string &type) {
    std::vector<Mesh> meshes;
    if (!AssetLoader::loadModel(path, &meshes))
        return NULL;
    else
        return new Drawable(meshes, type);
}

Drawable *DrawableFactory::createWaterTile(glm::vec3 position, int scale, glm::vec3 color) {
    Drawable *tmp;
    tmp = createPrimitive(PrimitiveType::QUAD, SHADER_TYPE_WATER);
    tmp->addColor(glm::vec4(color, 0.6));
    glm::vec3 scaleVec = glm::vec3(scale, 1, scale);
    tmp->transform(&scaleVec, &position, NULL);

    return tmp;
}

Drawable *DrawableFactory::createLight(const std::string &path, const std::string &type) {
    std::vector<Mesh> meshes;
    if (!AssetLoader::loadModel(path, &meshes))
        return NULL;
    else
        return new Light(meshes, type);
}

Drawable *DrawableFactory::createLight(PrimitiveType prim, std::string type, int size) {
    Mesh mesh;
    if (!createPrimitiveMesh(prim, &mesh))
        return NULL;
    else
        return new Light(mesh, type);
}