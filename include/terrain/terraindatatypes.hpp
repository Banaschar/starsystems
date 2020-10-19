#ifndef TERRAINDATATYPES_HPP
#define TERRAINDATATYPES_HPP

#include <glm/gtx/hash.hpp>
#include <vector>
#include <unordered_map>

class Drawable;
class Texture;
class TerrainNode_;

typedef std::vector<Drawable *> DrawableList;
typedef std::vector<Texture> TextureList;
typedef std::unordered_map<int, std::vector<Drawable *>> IndexedDrawableListMap;
typedef std::unordered_map<int, std::vector<Texture>> IndexedTextureListMap;

enum class TerrainType {DEFAULT, SPHERE, PLANE, CDLOD};

struct TerrainObjectAttributes {
    glm::vec3 bodyOrigin;
    float bodyRadius;
    bool hasAtmosphere;
    bool hasWater;
    float atmosphereHeight;
    ShaderType waterShaderType;
    ShaderType landShaderType;
};

class TerrainDrawData {
public:
    int size = 0;
    virtual TextureList &getGlobalTextureList() = 0;
    virtual TextureList &getTextureListAtIndex(int i) = 0;
    virtual DrawableList &getDrawableListAtIndex(int i) = 0;
};

struct TerrainObjectRenderData {
    TerrainDrawData *land = nullptr;
    TerrainDrawData *water = nullptr;
    TerrainObjectAttributes *attributes = nullptr;
};

typedef std::vector<TerrainObjectRenderData> TerrainRenderDataVector;

struct HeightMapCreationData {
    glm::vec2 cornerPos;
    int size;
    int currentHeightMapIndex;
    HeightMapCreationData(glm::vec2 &corner, int sizeC, int index) 
        : cornerPos(corner), size(sizeC), currentHeightMapIndex(index) {}
};

typedef std::unordered_map<int, std::vector<TerrainNode_ *>> IndexedTerrainNodeListMap;
typedef std::vector<HeightMapCreationData> HeigthMapCreationList;

#endif