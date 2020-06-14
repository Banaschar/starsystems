#ifndef SCENE_H
#define SCENE_H

#include <vector>

#include "drawable.hpp"
#include "game.hpp"
#include "renderer.hpp"

class Scene {
public:
    Scene(Game *game, Renderer *renderer);
    ~Scene();
    void update();
    void render();
    void setAutoRotate(bool value);
private:
    void setupScene();
    Game *game_;
    Renderer *renderer_;
    std::vector<Drawable*> &lights_;
    std::vector<Drawable*> &terrain_;
    std::vector<Drawable*> &entities_;
    std::vector<Drawable*> &water_;
    std::vector<Drawable*> &sky_;
    bool autoRotate_;
};
#endif