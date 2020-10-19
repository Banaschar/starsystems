#ifndef TEXTURELOADER_H
#define TEXTURELOADER_H

#include <string>
#include <vector>

#include "meshdatatypes.hpp"

class TextureLoader {
  public:
    static Texture loadTextureFromFile(const char *path, std::string name);
    static unsigned int loadTextureFromFile(const char *path);
    static unsigned int createTextureFromArray(unsigned char *data, int width, int height, int size);

    static Texture loadCubeMap(std::vector<std::string> textures, std::string name);
    static unsigned int loadCubeMap(std::vector<std::string> textures);

    static unsigned int loadDds(const char *path);
};
#endif