#ifndef COLORGENERATOR_H
#define COLORGENERATOR_H

#include <glm/glm.hpp>
#include <random>

#define DEFAULT_COLOR_SPREAD 0.45f
#define DEFAULT_COLOR_PALETTE std::vector<glm::vec4> {       \
                                glm::vec4(201, 178, 99, 1),  \
                                glm::vec4(135, 184, 82, 1),  \
                                glm::vec4(80, 171, 93, 1),   \
                                glm::vec4(120, 120, 120, 1), \
                                glm::vec4(200, 200, 210, 1)} 


class ColorGenerator {
public:
    ColorGenerator(bool random = false);

    ColorGenerator(std::vector<glm::vec4> colorPalette, float spread);

    std::vector<glm::vec4> genColors(std::vector<float> &heights, 
                                        float dimension, 
                                        float amplitude = 1.0f);

    glm::vec4 getRandomColor();
private:
    std::vector<glm::vec4> colorPalette_;
    float spread_;
    float halfSpread_;
    float part_;
    std::default_random_engine randEng_;

    void initRandom();

    float getRandF();

    glm::vec4 calcColor(float height, float amplitude);

    glm::vec4 interpolateColors(glm::vec4 col1, glm::vec4 col2, float blend);
};
#endif