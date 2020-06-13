#ifndef SCENE_H
#define SCENE_H

#include <glm/glm.hpp>
#include <vector>
#include <map>
#include <string>

#include "view.hpp"
#include "drawable.hpp"
#include "model.hpp"
#include "game.hpp"
#include "shader.hpp"

class Scene {
public:
    Scene(Game *game, std::vector<Shader*> shaderList);
    ~Scene();
    void update();
    void render();
    void insertDrawable(Drawable *drawable);
    void setAutoRotate(bool value);
private:
    void setupShaderMap(std::vector<Shader*> shaderList);
    Game *game_;
    std::map<std::string, Shader*> shaderMap_;
    std::map<std::string, std::vector<Drawable*>> entityMap_;
    std::vector<Drawable*> &lightSources_;
    std::vector<Drawable*> &models_;
    std::vector<Drawable*> &water_;
    Drawable *skybox_;
    bool autoRotate_;
};
#endif