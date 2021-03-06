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

void Gui::update() {
    /* Update all gui elements. Should create an abstract class GuiElement (that inherits from drawable) with an update function. */
    ;
}
/*
 * Adds a new gui element to be rendered.
 * TODO: Keep track of gui elements and warn if they overlap
 */
void Gui::addGuiElement(Texture tex, glm::vec2 position, glm::vec2 scale) {
    Drawable *tmp = DrawableFactory::createTexturedPrimitive(PrimitiveType::QUAD2D, ShaderType::SHADER_TYPE_GUI, tex);
    glm::vec3 tmptrans = glm::vec3(position.x, position.y, 0.0f);
    if (scale.x == 0 && scale.y == 0)
        scale = glm::vec2(1,1);
    glm::vec3 tmpscale = glm::vec3(scale.x, scale.y, 0.0f);
    tmp->transform(&tmpscale, &tmptrans, NULL);
    guiElements_.push_back(tmp);
}

std::vector<Drawable *> *Gui::getGuiElements() {
    return &guiElements_;
}