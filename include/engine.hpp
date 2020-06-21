#ifndef ENGINE_H
#define ENGINE_H

#include "global.hpp"
#include "drawable.hpp"
#include "drawablefactory.hpp"
#include "assetloader.hpp"
#include "game.hpp"
#include "scene.hpp"
#include "view.hpp"
#include "gui.hpp"
#include "terrain.hpp"
#include "terrainchunk.hpp"
#include "terrainquadtree.hpp"

struct GLFWwindow;

class Engine {
public:
    Engine(int width, int height, const std::string name);
    ~Engine();
    GLFWwindow* getWindow();
    void addScene(Scene *scene);
    void render();
private:
    Scene *scene_;
    GLFWwindow *window_ = NULL;
    void initWindow(int width, int height, const std::string name);
    void render_();
};
#endif