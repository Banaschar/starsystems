#include "assetloader.hpp"

#include <iostream>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <glm/glm.hpp>

#include "textureloader.hpp"

void createMeshes(aiNode *node, const aiScene *scene, std::vector<Mesh*> *meshes);
Mesh *processMesh(aiMesh *mesh, const aiScene *scene);
std::vector<Texture> loadMatTexture(aiMaterial *mat, aiTextureType type, std::string typeName);

bool AssetLoader::loadModel(const std::string &path, std::vector<Mesh*> *meshes) {
    std::string delimiter = ".";
    std::string extension = path.substr(path.find(delimiter) + 1, path.size());

    if (extension == "obj")
        return loadObj(path.c_str(), meshes);
    else
        std::cout << "Unkown file extension: " << extension << std::endl;

    return false;
}

bool AssetLoader::loadObj(const char *path, std::vector<Mesh*> *meshes) {

    Assimp::Importer importer;
    // Set second argument to 0 for no post processing
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
    if (!scene) {
        fprintf(stderr, "%s", importer.GetErrorString());
        getchar();
        return false;
    }

    // createMeshes(scene->mRootNode, scene, meshes);
    aiMesh *mesh = scene->mMeshes[0];
    meshes->push_back(processMesh(mesh, scene));

    if (meshes->size() == 0)
        return false;
    else
        return true;
}

void createMeshes(aiNode *node, const aiScene *scene, std::vector<Mesh*> *meshes) {
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes->push_back(processMesh(mesh, scene));
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        createMeshes(node->mChildren[i], scene, meshes);
    }
}

Mesh *processMesh(aiMesh *mesh, const aiScene *scene) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
        glm::vec3 vector;
        // pos
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.position = vector;
        // normal
        vector.x = mesh->mNormals[i].x;
        vector.y = mesh->mNormals[i].y;
        vector.z = mesh->mNormals[i].z;
        vertex.normal = vector;

        if (mesh->mTextureCoords[0]) {
            glm::vec2 vec;
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.textureCoords = vec;
        } else {
            vertex.textureCoords = glm::vec2(0.0f, 0.0f);
        }

        // TODO: read tangent and bitangent
        vertices.push_back(vertex);
    }

    // indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    // process materials
    aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

    std::vector<Texture> diffuseMaps = loadMatTexture(material, aiTextureType_DIFFUSE, "tex_diffuse");
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

    std::vector<Texture> specularMaps = loadMatTexture(material, aiTextureType_SPECULAR, "tex_specular");
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

    std::vector<Texture> normalMaps = loadMatTexture(material, aiTextureType_HEIGHT, "tex_normal");
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

    std::vector<Texture> heightMaps = loadMatTexture(material, aiTextureType_AMBIENT, "tex_height");
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

    return new Mesh(vertices, textures, indices);
}

/*
 * Loads textures specified in model files
 * TODO: Make sure not to load textures that have already been loaded
 */
std::vector<Texture> loadMatTexture(aiMaterial *mat, aiTextureType type, std::string typeName) {
    std::vector<Texture> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);
        Texture texture;
        texture.id = TextureLoader::loadTextureFromFile(str.C_Str());
        texture.type = typeName;
        texture.path = str.C_Str();
        textures.push_back(texture);
    }

    return textures;
}