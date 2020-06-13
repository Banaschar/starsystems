#ifndef WATERTILES_H
#define WATERTILES_H

#include "drawable.hpp"
#include "model.hpp"
#include "primitives.hpp"

class WaterTiles : Drawable {
public:
    WaterTiles(Shader shader) : shader_(shader) {
        
    }

    addWaterTile(glm::vec3 pos) {
        ;
    }
private:
    Shader shader_;
    std::vector<Drawable*> waterTiles_;
}
#endif;