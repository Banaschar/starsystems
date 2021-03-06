#ifndef ASSETLOADER_H
#define ASSETLOADER_H

#include <string>
#include <vector>

#include "mesh.hpp"

class AssetLoader {
  public:
    static bool loadModel(const std::string &path, std::vector<Mesh*> *meshes);

  private:
    static bool loadObj(const char *path, std::vector<Mesh*> *meshes);
};
#endif