#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <iostream>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include "assetloader.hpp"
#include "mesh.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// Macros for DDS loader
#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII

void createMeshes(aiNode *node, const aiScene *scene, std::vector<Mesh> *meshes);
Mesh processMesh(aiMesh *mesh, const aiScene *scene);
std::vector<Texture> loadMatTexture(aiMaterial *mat, aiTextureType type, std::string typeName);

bool loadObj(const char* path, std::vector<Mesh> *meshes) {

    Assimp::Importer importer;
    // Set second argument to 0 for no post processing
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
    if (!scene) {
        fprintf(stderr, "%s", importer.GetErrorString());
        getchar();
        return false;
    }

    //createMeshes(scene->mRootNode, scene, meshes);
    aiMesh *mesh = scene->mMeshes[0];
    meshes->push_back(processMesh(mesh, scene));

    if (meshes->size() == 0)
        return false;
    else
        return true;
}

void createMeshes(aiNode *node, const aiScene *scene, std::vector<Mesh> *meshes) {
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes->push_back(processMesh(mesh, scene));
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        createMeshes(node->mChildren[i], scene, meshes);
    }
}

Mesh processMesh(aiMesh *mesh, const aiScene *scene) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
        glm::vec3 vector;
        //pos
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.position = vector;
        //normal
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

    //process materials
    aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

    std::vector<Texture> diffuseMaps = loadMatTexture(material, aiTextureType_DIFFUSE, "tex_diffuse");
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

    std::vector<Texture> specularMaps = loadMatTexture(material, aiTextureType_SPECULAR, "tex_specular");
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

    std::vector<Texture> normalMaps = loadMatTexture(material, aiTextureType_HEIGHT, "tex_normal");
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

    std::vector<Texture> heightMaps = loadMatTexture(material, aiTextureType_AMBIENT, "tex_height");
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

    return Mesh(vertices, textures, indices);
}

// TODO: Make sure not to load textures that have already been loaded
std::vector<Texture> loadMatTexture(aiMaterial *mat, aiTextureType type, std::string typeName) {
    std::vector<Texture> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);
        Texture texture;
        texture.id = loadTextureFromFile(str.C_Str());
        texture.type = typeName;
        texture.path = str.C_Str();
        textures.push_back(texture);
    }

    return textures;
}

/*
 * TODO: 
 * directory in texture path
 * glTex parameter -> define more variable somewhere else
 */
unsigned int loadTextureFromFile(const char *path) {
    int width, height, nrChannels;
    unsigned int ret = 0;
    unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);

    if (data) {
        unsigned int texture;
        glGenTextures(1, &texture);

        GLenum format;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);

        ret = texture;
    } else {
        std::cout << "Could not load texture at: " << path << std::endl;
        stbi_image_free(data);
    }

    return ret;
}

/*
 * TODO: glTex parameters
 */
Texture loadCubeMap(std::vector<std::string> textures) {
    unsigned char *data;
    int width, height, nrChannels;
    Texture texture;
    unsigned int textureId;

    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);

    for (unsigned int i = 0; i < textures.size(); i++) {
        data = stbi_load(textures[i].c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            GLenum format;
            if (nrChannels == 1)
                format = GL_RED;
            else if (nrChannels == 3)
                format = GL_RGB;
            else if (nrChannels == 4)
                format = GL_RGBA;

            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        } else {
            std::cout << "Could not load cube map texture from: " << textures[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    texture.id = textureId;
    texture.type = "cubemap";
    return texture;
}

unsigned int loadDds(const char *path) {
    unsigned char header[124];

    FILE *fp; 
 
    /* try to open the file */ 
    fp = fopen(path, "rb"); 
    if (fp == NULL){
        printf("%s could not be opened. Are you in the right directory ? Don't forget to read the FAQ !\n", path); getchar(); 
        return 0;
    }
   
    /* verify the type of file */ 
    char filecode[4]; 
    fread(filecode, 1, 4, fp); 
    if (strncmp(filecode, "DDS ", 4) != 0) { 
        fclose(fp); 
        return 0; 
    }
    
    /* get the surface desc */ 
    fread(&header, 124, 1, fp); 

    unsigned int height      = *(unsigned int*)&(header[8 ]);
    unsigned int width       = *(unsigned int*)&(header[12]);
    unsigned int linearSize  = *(unsigned int*)&(header[16]);
    unsigned int mipMapCount = *(unsigned int*)&(header[24]);
    unsigned int fourCC      = *(unsigned int*)&(header[80]);

 
    unsigned char * buffer;
    unsigned int bufsize;
    /* how big is it going to be including all mipmaps? */ 
    bufsize = mipMapCount > 1 ? linearSize * 2 : linearSize; 
    buffer = (unsigned char*)malloc(bufsize * sizeof(unsigned char)); 
    fread(buffer, 1, bufsize, fp); 
    /* close the file pointer */ 
    fclose(fp);

    unsigned int components  = (fourCC == FOURCC_DXT1) ? 3 : 4; 
    unsigned int format;
    switch(fourCC) 
    { 
    case FOURCC_DXT1: 
        format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT; 
        break; 
    case FOURCC_DXT3: 
        format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT; 
        break; 
    case FOURCC_DXT5: 
        format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT; 
        break; 
    default: 
        free(buffer); 
        return 0; 
    }

    // Create one OpenGL texture
    unsigned int textureID;
    glGenTextures(1, &textureID);

    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, textureID);
    glPixelStorei(GL_UNPACK_ALIGNMENT,1);   
    
    unsigned int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16; 
    unsigned int offset = 0;

    /* load the mipmaps */ 
    for (unsigned int level = 0; level < mipMapCount && (width || height); ++level) 
    { 
        unsigned int size = ((width+3)/4)*((height+3)/4)*blockSize; 
        glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height,  
            0, size, buffer + offset); 
     
        offset += size; 
        width  /= 2; 
        height /= 2; 

        // Deal with Non-Power-Of-Two textures. This code is not included in the webpage to reduce clutter.
        if(width < 1) width = 1;
        if(height < 1) height = 1;

    } 

    free(buffer); 

    return textureID;
}