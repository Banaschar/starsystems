#ifndef SCENE_H
#define SCENE_H

#include <glm/glm.hpp>

#include "view.hpp"
#include "drawable.hpp"
#include "model.hpp"

class Scene {
public:
    Scene(View view, Model light);
    void update();
    void render();
    void addModel(Drawable *model);
    void setAutoRotate(bool value);
private:
    View view_;
    std::vector<Drawable*> models_;
    Model lightSource_;
    bool autoRotate_;
};
#endif