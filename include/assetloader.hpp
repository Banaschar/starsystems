#ifndef ASSETLOADER_H
#define ASSETLOADER_H

#include <GLFW/glfw3.h>

#include "mesh.hpp"

bool loadObj(const char *path, std::vector<Mesh> *meshes);

GLuint loadDds(const char *path);

#endif