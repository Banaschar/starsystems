#include "gui.hpp"

Gui::Gui() {}
Gui::Gui(unsigned int textureID, glm::vec2 position, glm::vec2 scale) {
    Texture t;
    t.id = textureID;
    t.type = "texture_gui";
    addGuiElement(t, position, scale);
}

Gui::Gui(Texture tex, glm::vec2 position, glm::vec2 scale) {
    addGuiElement(tex, position, scale);
}

Gui::~Gui() {
    for (Drawable *m : guiElements_) {
        delete m;
    }
}
/*
 * Adds a new gui element to be rendered.
 * TODO: Keep track of gui elements and warn if they overlap
 */
void Gui::addGuiElement(Texture tex, glm::vec2 position, glm::vec2 scale) {
    Drawable *tmp = DrawableFactory::createTexturedPrimitive(PrimitiveType::QUAD2D, "gui", tex);
    glm::vec3 tmptrans = glm::vec3(position.x, position.y, 0.0f);
    glm::vec3 tmpscale = glm::vec3(scale.x, scale.y, 0.0f);
    tmp->transform(&tmpscale, &tmptrans, NULL);
    guiElements_.push_back(tmp);
}

void Gui::update(Game *game) {
    ;
}

std::vector<Drawable *> *Gui::getGuiElements() {
    return &guiElements_;
}