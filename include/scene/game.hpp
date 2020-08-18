#ifndef GAME_H
#define GAME_H

#include <glm/glm.hpp>
#include <iostream>
#include <vector>

#include "drawable.hpp"
#include "light.hpp"
#include "view.hpp"
#include "terrainquadtree.hpp"

class Game {
  public:
    Game(View view);
    ~Game();

    void addEntity(Drawable *entity);

    void addLight(Drawable *light);

    void addSun(Drawable *sun);

    void addSky(Drawable *sky);

    void addTerrain(Drawable *terrain);

    void addWater(Drawable *water);

    void update();

    View &getView();

    std::vector<Drawable *> &getLights();

    std::vector<Drawable *> &getEntities();

    std::vector<Drawable *> &getTerrain();

    std::vector<Drawable *> &getWater();

    Drawable *getSun();

    Drawable *getLightSource(int index = 0);

    std::vector<Drawable *> &getSky();

    float getWaterLevel();

  private:
    View view_;
    Drawable *sun_ = NULL;
    float waterLevel_ = 0.0;
    TerrainQuadTree *terrainQuadTree_ = NULL;
    std::vector<Drawable *> sky_;
    std::vector<Drawable *> lights_;
    std::vector<Drawable *> terrain_;
    std::vector<Drawable *> entities_;
    std::vector<Drawable *> water_;
};
#endif