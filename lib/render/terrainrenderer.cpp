#include "oglheader.hpp"
#include "terrainrenderer.hpp"
#include "textureloader.hpp"
#include "global.hpp"

TerrainRenderer::TerrainRenderer(Shader *shader, VaoRenderer *vaoRenderer)
    : shader_(shader), vaoRenderer_(vaoRenderer) {
    setupTextures();
}

void TerrainRenderer::setupTextures() {
    Texture tex;
    tex.type = "texture_diffuse";
    tex.id = TextureLoader::loadTextureFromFile("assets/seaGround.jpg");
    textures_.push_back(tex);
    tex.id = TextureLoader::loadTextureFromFile("assets/sand256.tga");
    textures_.push_back(tex);
    tex.id = TextureLoader::loadTextureFromFile("assets/grass.tga");
    textures_.push_back(tex);
    tex.id = TextureLoader::loadTextureFromFile("assets/ground.tga");
    textures_.push_back(tex);
    tex.id = TextureLoader::loadTextureFromFile("assets/rock512.tga");
    textures_.push_back(tex);
    tex.id = TextureLoader::loadTextureFromFile("assets/snow512.tga");
    textures_.push_back(tex);
}

//TODO ceifert Remove this and setupTextures. Textures should only be provided by the TerrainObjectRenderData
void TerrainRenderer::bindTextures() {
    for (int i = 0; i < textures_.size(); i++) {
        shader_->bindTexture(textures_[i].type + std::to_string(i + 1), textures_[i].id);
    }
}

void TerrainRenderer::bindTextureList(TextureList &textureList) {
    for (Texture &tex : textureList) {
        shader_->bindTexture(tex.type, tex.id);
    }
}

void TerrainRenderer::render(TerrainRenderDataVector &renderDataVector, SceneRenderData &sceneData) {
    for (TerrainObjectRenderData &tObj : renderDataVector) {
        if (tObj.land->size)
            render_(tObj, sceneData);
    }
}

void TerrainRenderer::render_(TerrainObjectRenderData &renderData, SceneRenderData &sceneData) {
    shader_->use();

    bindTextureList(renderData.land->getGlobalTextureList());
    shader_->setSceneUniforms(sceneData, nullptr);

    for (int i = 0; i < renderData.land->size; ++i) {
        TextureList &texList = renderData.land->getTextureListAtIndex(i);
        int numTex = texList.size();
        bindTextureList(texList);

        for (Drawable *drawable : renderData.land->getDrawableListAtIndex(i)) {
            shader_->setDrawableUniforms(sceneData, drawable, nullptr);

            for (Mesh *mesh : drawable->getMeshes()) {
                vaoRenderer_->draw(mesh);
            }
        }

        shader_->decreaseTextureCount(numTex);
    }

    shader_->end();
}