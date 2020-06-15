#ifndef TERRAIN_H
#define TERRAIN_H

#include "global.hpp"
#include "generator.hpp"
#include "model.hpp"
#include "assetloader.hpp"

#define DEFAULT_DIMENSION 200

class Terrain : public Drawable {
public:
    Terrain(TerrainGenerator terrainGen, int dimension = DEFAULT_DIMENSION) {
        model_ = new Model(terrainGen.generateTerrain(dimension), SHADER_TYPE_TERRAIN);
        amplitude_ = terrainGen.getPerlinNoise().getAmplitude();
        setupTextures();
    }

    Terrain(int dimension = DEFAULT_DIMENSION) {
        TerrainGenerator terrainGen = TerrainGenerator();
        model_ = new Model(terrainGen.generateTerrain(dimension), SHADER_TYPE_TERRAIN);
        amplitude_ = terrainGen.getPerlinNoise().getAmplitude();
        setupTextures();
    }

    ~Terrain() {
        delete model_;
    }

    void setupTextures() {
        Texture tex;
        tex.type = "texture_diffuse";
        tex.id = loadTextureFromFile("sand256.tga");
        model_->getMesh().addTexture(tex);
        tex.id = loadTextureFromFile("grass.tga");
        model_->getMesh().addTexture(tex);
        tex.id = loadTextureFromFile("ground.tga");
        model_->getMesh().addTexture(tex);
        tex.id = loadTextureFromFile("rock512.tga");
        model_->getMesh().addTexture(tex);
        tex.id = loadTextureFromFile("snow512.tga");
        model_->getMesh().addTexture(tex);
    }

    void draw(Shader *shader) {
        model_->draw(shader);
    }
    void update(Game *game) {
        model_->update(game);
    }
    glm::vec3 getPosition(int index = 0) {
        return model_->getPosition(index);
    }
    glm::mat4 getMvp(int index = 0) {
        return model_->getMvp(index);
    }
    glm::mat4 getModelMatrix(int index = 0) {
        return model_->getModelMatrix(index);
    }
    glm::mat3 getNormalMatrix(int index = 0) {
        return model_->getNormalMatrix(index);
    }
    std::string type() {
        return model_->type();
    }

    float getAmplitude() {
        return amplitude_;
    }
private:
    float amplitude_;
    Model *model_;
};

class TerrainRenderer {
public:
    TerrainRenderer(Shader *shader) : shader_(shader) {}
    void render(std::vector<Drawable*> terrains, Game *game) {
        shader_->use();
        for (Drawable* drawable : terrains) {
            Terrain *terrain = static_cast<Terrain*> (drawable);
            shader_->uniform("amplitude", terrain->getAmplitude());
            drawable->update(game);
            shader_->prepare(drawable, game);
            drawable->draw(shader_);
        }
    }
private:
    Shader *shader_;
};
#endif