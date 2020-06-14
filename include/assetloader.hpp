#ifndef ASSETLOADER_H
#define ASSETLOADER_H

#include <GLFW/glfw3.h>

#include "mesh.hpp"

bool loadObj(const char *path, std::vector<Mesh> *meshes);

unsigned int loadDds(const char *path);

unsigned int loadPngTexture(const char *path);

unsigned int loadTextureFromFile(const char *path);

Texture loadCubeMap(std::vector<std::string> textures);

#endif