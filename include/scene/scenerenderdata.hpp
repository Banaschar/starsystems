#ifndef SCENERENDERDATA_HPP
#define SCENERENDERDATA_HPP

class View;
class Drawable;

struct SceneRenderData {
    View *view = nullptr;
    Drawable *sun = nullptr;
};
#endif