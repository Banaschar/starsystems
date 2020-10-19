#ifndef TERRAINMANAGER_H
#define TERRAINMANAGER_H

#include "terraingenerator.hpp"
#include "terrainnode.hpp"
#include "view.hpp"
#include "terraindatatypes.hpp"

class BaseDrawData : public TerrainDrawData {
public:
    BaseDrawData() {
        size = 1;
    }

    TextureList &getGlobalTextureList() {
        return globalTextureList;
    }

    TextureList &getTextureListAtIndex(int i) {
        return textureList;
    }

    DrawableList &getDrawableListAtIndex(int i) {
        return drawableList;
    }

    TextureList globalTextureList;
    TextureList textureList;
    DrawableList drawableList;
};

class TerrainObject {
public:
  virtual void update(View *view, TerrainObjectRenderData *terrainObjectRenderData) = 0;
};

class TerrainManager {
public:
    TerrainManager();
    TerrainManager(TerrainObject *obj);
    ~TerrainManager();

    void addTerrainObject(TerrainObject *obj);
    void update(View *view);
    TerrainRenderDataVector &getTerrainRenderData();
    TerrainType getType();

private:
    TerrainType type_;
    std::vector<TerrainObject *> terrainObjectList_;
    TerrainRenderDataVector terrainObjectRenderDataVector_;
};

class TerrainChunkTree : public TerrainObject {
public:
    TerrainChunkTree();
    void update(View *view, TerrainObjectRenderData *terrainObjectRenderData);
    TerrainDrawData *getDrawData();
    void addTerrainChunk(Drawable *drawable);

private:
    BaseDrawData terrainDrawData;
    TerrainObjectAttributes terrainAttributes_;
};

/*
 * Implementation of CDLOD
 * https://github.com/fstrugar/CDLOD
 */
class TerrainCDLODTree : public TerrainObject {
public:
  TerrainCDLODTree(TerrainGenerator *terrainGen);
  ~TerrainCDLODTree();
  void update(View *view, DrawableList *terrainList, DrawableList *waterList);
  
private:
    TerrainGenerator *terrainGen_;
    std::vector<float> ranges_;
    VertexAttributeData rangeAttribData_;
    std::vector<glm::vec3> instanceVecMorphAttribs_;
    std::vector<std::vector<TerrainNode_ *>> grid_;
    std::vector<TerrainNode_ *> nodesToDraw_;

    std::unordered_map<glm::vec2, TerrainNode_ *> rootNodeMap_;
    std::unordered_map<int, std::vector<TerrainNode_ *>> nodeDrawMap_;
    std::unordered_map<int, HeightMap *> heightMaps_;
    int heightMapIndex_ = 0;
    int lodLevelCount_;
    int rootNodeDimension_;
    Drawable *basePatch_;

    float getPrevRange(int lodLevel);
    void createNode(glm::vec3 rootNodeOrigin, glm::vec3 rootNodeAxis);
};

class PlanetDrawData : public TerrainDrawData {
public:
    TextureList &getGlobalTextureList() {
        return globalTextureList;
    }

    TextureList &getTextureListAtIndex(int i) {
        return *(listOfTextureLists[i]);
    }

    DrawableList &getDrawableListAtIndex(int i) {
        return *(listOfDrawableLists[i]);
    }

    TerrainObjectAttributes &getTerrainAttributes() {
        return planetAttributes;
    }
private:
  std::vector<DrawableList *> listOfDrawableLists;
  std::vector<TextureList *> listOfTextureLists;
  TextureList globalTextureList;
  TerrainObjectAttributes planetAttributes; // Holds things like origin, radius, hasAtmosphere (for global uniforms and stuff)
};

struct MeshInstanceData {
    IndexedDrawableListMap baseMeshListMap;
    std::unordered_map<int, std::vector<glm::vec3>> instanceAttribListMap;
    std::unordered_map<int, VertexAttributeData> instanceAttributeDataMap;
    std::vector<glm::vec3>::iterator attribIt;
    std::vector<glm::vec3> *currVec;

    void createNewInstance(int index, Drawable *baseMesh) {
        baseMeshListMap[index] = DrawableList(1, baseMesh);
        instanceAttribListMap[index];
        instanceAttributeDataMap[index];
    }

    void updateInstanceSize(int index, int size) {
        baseMeshListMap[index][0]->updateInstanceSize(size);
        instanceAttribListMap[index].resize(size);
        instanceAttributeDataMap[index].size = size;

        currVec = &instanceAttribListMap[index];
        attribIt = currVec->begin();
    }

    void insertAttribute(float x, float y, float z) {
        currVec->emplace(attribIt++, x, y, z);
    }

    void finishInstance(int index) {
        instanceAttributeDataMap[index].data = static_cast<void *>(instanceAttribListMap[index].data());
        baseMeshListMap[index][0]->updateMeshInstances(&instanceAttributeDataMap[index]);
    }
};

class PlanetTree : public TerrainObject {
public:
    PlanetTree();
    ~PlanetTree();
    void update(View *view, TerrainObjectRenderData *terrainObjectRenderData);
    TerrainDrawData *getDrawData();

private:
    int planetOrigin_;
    int planetRadius_;
    int rootNodeDimension_;
    int lodLevelCount_;
    int leafNodeSize_;
    std::vector<float> planetRanges_;
    std::vector<TerrainNode_ *> rootNodeList_;
    IndexedTerrainNodeListMap selectedNodes_;
    HeigthMapCreationList heightMapCreateList_; // vec4(mapIndex, vec3 pos)
    MeshInstanceData meshInstanceData_;
    std::unordered_map<int, HeightMap *> heightMaps_;
    IndexedTextureListMap heightMapTextures_;
    PlanetDrawData planetDrawData_;
    TerrainObjectAttributes planetAttributes_;

    void initPlanet();
    void initGlobalTextures();
    TerrainNode_ *createRootNode(glm::vec2 &cornerPos, glm::vec3 &axis);
    float getPrevRange(int lodLevel);
    glm::vec2 vec3ToVec2CubeSide(glm::vec3 &pos, glm::vec3 &axis);
    glm::vec3 vec2ToVec3CubeSide(glm::vec2 &pos, glm::vec3 &axis);
    glm::vec3 getRotationAxis(glm::vec3 &axis, float *degree);
};
#endif