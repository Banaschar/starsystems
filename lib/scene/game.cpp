#include "game.hpp"

Game::Game(View view) : view_(view) {
    /* TODO: rethink this
     * This is only here because I copy the view object in here,
     * and therefore the callback for glfw points to the wrong view object.
     * Could be solved by just getting a pointer to view instead of copying the
     * object. Would also be more harmonic, as the all passed around objects would need
     * the "->" accessor.
     */
    view_.setupInput();
}

Game::~Game() {
    if (terrainManager_)
        delete terrainManager_;
    else {
        for (Drawable *m : terrain_) {
            delete m;
        }
        for (Drawable *m : water_) {
            delete m;
        }
    }
    for (Drawable *m : entities_) {
        delete m;
    }
    for (Drawable *m : lights_) {
        delete m;
    }

    for (Drawable *m : sky_) {
        delete m;
    }
}

void Game::update() {
    view_.update();
    /*
     * Update terrain tree and update drawable list
     * if neccessary
     */ 
    if (terrainManager_)
        terrainManager_->update(view_.getCameraPosition(), view_.getCameraDirection(), &terrain_, &water_);
}

void Game::addTerrainManager(TerrainManager *terrainManager) {
    if (terrainManager_)
        delete terrainManager_;
    terrainManager_ = terrainManager;
}

void Game::addEntity(Drawable *entity) {
    entities_.push_back(entity);
}

void Game::addLight(Drawable *light) {
    Light *tmp = dynamic_cast<Light *>(light);
    if (tmp)
        lights_.push_back(light);
    else
        std::cout << "Model added as light is not actually of class Light" << std::endl;
}

void Game::addSun(Drawable *sun) {
    Light *tmp = dynamic_cast<Light *>(sun);
    if (tmp) {
        sun_ = sun;
        if (tmp->hasModel())
            lights_.push_back(sun);
    } else {
        std::cout << "Model added as sun is not actually of class Light" << std::endl;
    }
}

void Game::addSky(Drawable *sky) {
    sky_.push_back(sky);
}

void Game::addTerrain(Drawable *terrain) {
    terrain_.push_back(terrain);
}

void Game::addWater(Drawable *water) {
    water_.push_back(water);
    waterLevel_ = water->getPosition().y;
}

View &Game::getView() {
    return view_;
}

std::vector<Drawable *> &Game::getLights() {
    return lights_;
}

std::vector<Drawable *> &Game::getEntities() {
    return entities_;
}

std::vector<Drawable *> &Game::getTerrain() {
    return terrain_;
}

std::vector<Drawable *> &Game::getWater() {
    return water_;
}

Drawable *Game::getSun() {
    if (sun_)
        return sun_;
    else {
        if (lights_.size())
            return lights_.at(0);
        else {
            fprintf(stdout, "[GAME::getSun] Warning: Requested sun but the sun does not exist!\n");
            return NULL;
        }
    }
}

Drawable *Game::getLightSource(int index) {
    return lights_.at(index);
}

std::vector<Drawable *> &Game::getSky() {
    return sky_;
}

float Game::getWaterLevel() {
    return waterLevel_;
}