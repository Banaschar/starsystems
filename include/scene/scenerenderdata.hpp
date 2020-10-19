#ifndef SCENERENDERDATA_HPP
#define SCENERENDERDATA_HPP

#include <glm/glm.hpp>
#include <vector>

class View;
class Drawable;

struct SceneRenderData {
    View *view = nullptr;
    Drawable *sun = nullptr;
    glm::vec4 *clipPlane = nullptr;
    std::vector<Drawable *> sceneLightList;
};
#endif