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
    //TODO: Refactor, should be called something like provideFrameRenderData
  virtual ~TerrainObject() {}
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
    void update(View *view, TerrainObjectRenderData *terrainObjectRenderData) override;
    TerrainDrawData *getDrawData();
    void addTerrainChunk(Drawable *drawable);
    ~TerrainChunkTree() override;

  private:
    BaseDrawData terrainDrawData_;
    TerrainObjectAttributes terrainAttributes_;
};

class CdlodDrawData : public TerrainDrawData {
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

  std::vector<DrawableList *> listOfDrawableLists;
  std::vector<TextureList *> listOfTextureLists;
  TextureList globalTextureList;
};

struct MeshInstanceData {
    IndexedDrawableListMap baseMeshListMap;
    std::unordered_map<int, std::vector<glm::vec3>> instanceAttribListMap;
    std::unordered_map<int, VertexAttributeData> instanceAttributeDataMap;
    std::vector<glm::vec3>::iterator attribIt;
    std::vector<glm::vec3> *currAttribVec;
    Drawable *currBaseMesh;

    void createNewInstance(int mapIndex) {
        if (baseMeshListMap.find(mapIndex) == baseMeshListMap.end()) {
            fprintf(stdout, "[MESHINSTANCEDATA::createNewInstance] Error: New instance has no base mesh\n");
            return;
        }

        instanceAttribListMap[mapIndex];
        instanceAttributeDataMap[mapIndex];
    }

    void updateInstanceSize(int mapIndex, int size) {
        baseMeshListMap[mapIndex][0]->updateInstanceSize(size);
        instanceAttribListMap[mapIndex].resize(size);
        VertexAttributeData *instanceAttribs = &instanceAttributeDataMap[mapIndex];
        instanceAttribs->size = size;
        instanceAttribs->numElements = 3;
        instanceAttribs->sizeOfDataType = sizeof(glm::vec3);

        currBaseMesh = baseMeshListMap[mapIndex][0];
        currAttribVec = &instanceAttribListMap[mapIndex];
        //attribIt = currAttribVec->begin();
    }

    void insertModelMatrix(int instanceIndex, glm::vec3 *scale, glm::vec3 *translate, glm::vec3 *rotate, float degree) {
        currBaseMesh->transform(instanceIndex, scale, translate, rotate, degree);
    }

    void insertAttribute(int instanceIndex, float x, float y, float z) {
        //currAttribVec->emplace(attribIt, x, y, z);
        //++attribIt;
        (*currAttribVec)[instanceIndex] = glm::vec3(x,y,z);
    }

    void finishInstance(int mapIndex) {
        instanceAttributeDataMap[mapIndex].data = static_cast<void *>(currAttribVec->data());
        currBaseMesh->updateMeshInstances(&instanceAttributeDataMap[mapIndex]);
    }
};

struct CdlodTreeData {
    std::vector<TerrainNode_ *> *rootNodes;
    std::unordered_map<int, HeightMap *> *heightMaps;
    IndexedTextureListMap *heightMapTextures;
    IndexedDrawableListMap *baseMeshListMap;
    TextureList *globalTextureList;
    std::vector<float> *ranges;
    int *heightMapIndex;
    int *leafNodeSize; 
    int *lodLevelCount;
};

/*
 * TODO: Remove. Add the methods as private virtual methods to CdlodTree (call it CdlodTreeBase)
 *              -> Override in sub classes -> NO NEED FOR CDLODTREEDAT. WRONG: Can't access private base class members. Have to make all members Protected instead
 */
class CdlodTreeImplementation {
public:
    /* Fill CdlodTreeData */
    virtual void createTree(CdlodTreeData &treeData) = 0;
    /* Called when a child requires a new heightmap */
    virtual void updateRootNodes(CdlodTreeData &treeData, std::vector<TerrainNode_ *> &heightMapDemandList) = 0;
};

class PlanetCdlodImplementation : public CdlodTreeImplementation {
public:
    PlanetCdlodImplementation(TerrainGenerator *terrainGen, TerrainObjectAttributes *terrainAttribs);
    void createTree(CdlodTreeData &treeData);
    void updateRootNodes(CdlodTreeData &treeData, std::vector<TerrainNode_ *> &heightMapDemandList);
private:
    int rootNodeDimension_;
    TerrainObjectAttributes *planetAttributes_;
    TerrainGenerator *terrainGen_;

    void createRootNode(CdlodTreeData &treeData, glm::vec2 &cornerPos, glm::vec3 &axis);
    glm::vec2 vec3ToVec2CubeSide(glm::vec3 &pos, glm::vec3 &axis);
};

class PlaneCdlodImplementation : public CdlodTreeImplementation {
public:
    PlaneCdlodImplementation(TerrainGenerator *terrainGen, TerrainObjectAttributes *terrainAttribs);
    void createTree(CdlodTreeData &treeData);
    /* TODO: Make changes in treeData thread safe. Need mutex for treeData.rootNodes */
    void updateRootNodes(CdlodTreeData &treeData, std::vector<TerrainNode_ *> &heightMapDemandList);
private:
    int rootNodeDimension_;
    TerrainGenerator *terrainGen_;
    TerrainObjectAttributes *planeAttributes_;
    void createRootNode(CdlodTreeData &treeData, glm::vec2 pos);
};

/*
 * Needs a background thread as garbage collector:
 * Need to remove instances in MeshInstanceData if the corresponding heightmap and drawable have been deleted
 * during a call to ->updateRootNodes();
 */
class CdlodTree : public TerrainObject {
public:
  CdlodTree(CdlodTreeImplementation *imple, TerrainObjectAttributes *attribs);
  ~CdlodTree() override;

  void update(View *view, TerrainObjectRenderData *terrainObjectRenderData) override;
  
private:
    int leafNodeSize_;
    int heightMapIndex_;
    int lodLevelCount_;
    std::vector<float> ranges_;
    std::vector<TerrainNode_ *> rootNodeList_; // List of root nodes
    std::unordered_map<int, HeightMap *> heightMaps_; // List of heightmaps
    IndexedTextureListMap heightMapTextures_; // Map of Texture lists with heightmap + normalmap
    CdlodDrawData landDrawData_; // out draw data for land
    CdlodDrawData waterDrawData_; // out draw data for water
    IndexedTerrainNodeListMap selectedNodes_; // Map filled each frame with TerrainNodes in range
    MeshInstanceData meshInstanceData_; // Data structures for mesh instances
    CdlodTreeData publicTreeData_; // struct to pass the relevant tree data to the implementing class
    std::vector<TerrainNode_ *> heightMapCreationList_; // List filled by node->lodSelect, heightMaps that need to be created
    TerrainObjectAttributes *terrainAttributes_; // Attributes provided by Constructor
    CdlodTreeImplementation *treeImplementation_;

    void init();
    void handleRootNodeUpdate(std::vector<TerrainNode_ *> creationList);
    glm::vec3 vec2ToVec3CubeSide(glm::vec2 &pos, glm::vec3 &axis);
    glm::vec3 getRotationAxis(glm::vec3 &axis, float *degree);
    float getPrevRange(int lodLevel);
};

class Planet : public TerrainObject {
public:
    Planet(TerrainGenerator *terrainGen);
    ~Planet() override;
    void update(View *view, TerrainObjectRenderData *terrainObjectRenderData) override;

private:
    CdlodTree *lodTree_;
    TerrainGenerator *terrainGen_;
    TerrainObjectAttributes terrainAttributes_;

    void initPlanet();
};

class EndlessPlane : public TerrainObject {
public:
    EndlessPlane(TerrainGenerator *terrainGen);
    ~EndlessPlane() override;
    void update(View *view, TerrainObjectRenderData *terrainObjectRenderData) override;

private:
    CdlodTree *lodTree_;
    TerrainGenerator *terrainGen_;
    TerrainObjectAttributes terrainAttributes_;
    void initPlane();
};
#endif