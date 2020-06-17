#ifndef TERRAIN_H
#define TERRAIN_H

#include "global.hpp"
#include "generator.hpp"
#include "model.hpp"
#include "assetloader.hpp"

#define DEFAULT_DIMENSION 200

class Terrain : public Drawable {
public:
    Terrain(TerrainGenerator terrainGen, int dimension = DEFAULT_DIMENSION) : dimension_(dimension){
        model_ = new Model(terrainGen.generateTerrain(dimension_), SHADER_TYPE_TERRAIN);
        amplitude_ = terrainGen.getPerlinNoise().getAmplitude();
    }

    Terrain(int dimension = DEFAULT_DIMENSION) : dimension_(dimension) {
        PerlinNoise pNoise = PerlinNoise(4, 10.0f, 0.01f);
        TerrainGenerator terrainGen = TerrainGenerator(pNoise);
        model_ = new Model(terrainGen.generateTerrain(dimension_), SHADER_TYPE_TERRAIN);
        amplitude_ = terrainGen.getPerlinNoise().getAmplitude();
    }

    ~Terrain() {
        delete model_;
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
    glm::vec3 getScale(int index = 0) {
        return model_->getScale(index);
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

    void transform(glm::vec3 *scale, glm::vec3 *translate, glm::vec3 *rotate, float degree = 0.0) {
        model_->transform(scale, translate, rotate, degree);
    }

    float getAmplitude() {
        return amplitude_;
    }

    int getDimension() {
        return dimension_;
    }
private:
    int dimension_;
    float amplitude_;
    Model *model_;
};

class TerrainRenderer {
public:
    TerrainRenderer(Shader *shader) : shader_(shader) {
        setupTextures();
    }

    void setupTextures() {
        Texture tex;
        tex.type = "texture_diffuse";
        tex.id = loadTextureFromFile("seaGround.jpg");
        textures_.push_back(tex);
        tex.id = loadTextureFromFile("sand256.tga");
        textures_.push_back(tex);
        tex.id = loadTextureFromFile("grass.tga");
        textures_.push_back(tex);
        tex.id = loadTextureFromFile("ground.tga");
        textures_.push_back(tex);
        tex.id = loadTextureFromFile("rock512.tga");
        textures_.push_back(tex);
        tex.id = loadTextureFromFile("snow512.tga");
        textures_.push_back(tex);
    }

    void bindTextures() {
        for (int i = 0; i < textures_.size(); i++) {
            shader_->bindTexture(textures_[i].type + std::to_string(i + 1), textures_[i].id);
        }
    }

    void render(std::vector<Drawable*> terrains, Game *game, glm::vec4 clipPlane) {
        shader_->use();
        shader_->uniform("clipPlane", clipPlane);
        shader_->uniform("waterLevel", game->getWaterLevel());
        bindTextures();
        for (Drawable* drawable : terrains) {
            Terrain *terrain = static_cast<Terrain*> (drawable);
            shader_->uniform("amplitude", terrain->getAmplitude());
            shader_->uniform("tiling", (float)terrain->getDimension() / 4.0f);
            drawable->update(game);
            shader_->prepare(drawable, game);
            drawable->draw(shader_);
        }
    }
private:
    Shader *shader_;
    std::vector<Texture> textures_;
};
#endif