#ifndef ASSETLOADER_H
#define ASSETLOADER_H

#include "mesh.hpp"

bool loadObj(const char *path, std::vector<Mesh> *meshes);

unsigned int loadDds(const char *path);

unsigned int loadPngTexture(const char *path);

unsigned int loadTextureFromFile(const char *path);

Texture loadCubeMap(std::vector<std::string> textures);

#endif