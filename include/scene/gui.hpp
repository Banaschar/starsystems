#ifndef GUI_H
#define GUI_H

#include <string>

#include "drawablefactory.hpp"

class Gui {
  public:
    Gui();
    Gui(unsigned int textureID, glm::vec2 position, glm::vec2 scale = glm::vec2(0.5f));
    Gui(Texture tex, glm::vec2 position, glm::vec2 scale = glm::vec2(0.5f));
    ~Gui();
    void addGuiElement(Texture tex, glm::vec2 position, glm::vec2 scale = glm::vec2(0.5f));
    std::vector<Drawable *> *getGuiElements();
    void update();

  private:
    std::vector<Drawable *> guiElements_;
};
#endif