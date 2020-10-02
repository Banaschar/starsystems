#ifndef ENGINE_H
#define ENGINE_H

#include "assetloader.hpp"
#include "drawable.hpp"
#include "drawablefactory.hpp"
#include "game.hpp"
#include "global.hpp"
#include "gui.hpp"
#include "scene.hpp"
#include "terrainmanager.hpp"
#include "view.hpp"

struct GLFWwindow;

class Engine {
  public:
    Engine(int width, int height, const std::string &name);
    ~Engine();
    GLFWwindow *getWindow();
    void addScene(Scene *scene);
    void render();

  private:
    Scene *scene_;
    GLFWwindow *window_ = NULL;
    void initWindow(int width, int height, const std::string &name);
    void initThreadPool();
    void render_();
};
#endif