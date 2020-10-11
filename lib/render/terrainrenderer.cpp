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

void TerrainRenderer::bindTextures() {
    for (int i = 0; i < textures_.size(); i++) {
        shader_->bindTexture(textures_[i].type + std::to_string(i + 1), textures_[i].id);
    }
}

void TerrainRenderer::render(std::vector<Drawable *> &terrains, Game *game, glm::vec4 &clipPlane) {
    if (terrains.empty())
        return;
    
    TerrainType type = game->getTerrainManagerType();
    if (type == TerrainType::SPHERE || type == TerrainType::PLANE) {
        glDepthFunc(GL_LEQUAL);
        shader_->use();
        shader_->uniform("clipPlane", clipPlane);
        shader_->uniform("waterLevel", game->getWaterLevel());
        bindTextures();
        for (Drawable *drawable : terrains) {
            if (drawable) {
                TerrainTile *terrain = static_cast<TerrainTile *>(drawable);
                shader_->uniform("amplitude", terrain->getAmplitude());
                shader_->uniform("tiling", (float)terrain->getDimension() / 2.0f);
                shader_->uniform("sphereRadius", terrain->getSphereRadius());
                shader_->uniform("sphereOrigin", terrain->getSphereOrigin());
                drawable->update(game);
                shader_->prepare(drawable, game);

                
                for (Mesh *mesh : drawable->getMeshes())
                    vaoRenderer_->draw(mesh);
            }
        }
        shader_->end();
        glDepthFunc(GL_LESS);
    } else if (type == TerrainType::CDLOD || type == TerrainType::DEFAULT) {
        shader_->use();
        shader_->handleMeshTextures(terrains[0]->getMeshes()[0]->getTextures());
        shader_->prepare(terrains[0], game);
        vaoRenderer_->draw(terrains[0]->getMeshes()[0]);
        shader_->end();
    }
}