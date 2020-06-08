#ifndef SCENE_H
#define SCENE_H

#include <glm/glm.hpp>
#include <vector>

#include "view.hpp"
#include "drawable.hpp"
#include "model.hpp"
#include "game.hpp"

class Scene {
public:
    Scene(Game game);
    ~Scene();
    void update();
    void render();
    void setAutoRotate(bool value);
private:
    Game game_;
    std::vector<Drawable*> &lightSources_;
    std::vector<Drawable*> &models_;
    bool autoRotate_;
};
#endif