#include "colorgenerator.hpp"
#include <ctime>

ColorGenerator::ColorGenerator(bool random) {
    if (random) {
        initRandom();
        spread_ = getRandF();
        for (int i = 0; i < 5; i++) {
            colorPalette_.push_back(getRandomColor());
        }
    } else {
        colorPalette_ = DEFAULT_COLOR_PALETTE;
    }
    spread_ = DEFAULT_COLOR_SPREAD;
    halfSpread_ = spread_ / 2.0f;
    part_ = 1.0f / (colorPalette_.size() - 1);
}

ColorGenerator::ColorGenerator(std::vector<glm::vec4> colorPalette, float spread)
    : colorPalette_(colorPalette), spread_(spread) {
    halfSpread_ = spread_ / 2.0f;
    part_ = 1.0f / (colorPalette_.size() - 1);
    initRandom();
}

std::vector<glm::vec4> ColorGenerator::genColors(std::vector<float> &heights, float dimension, float amplitude) {
    std::vector<glm::vec4> colors(heights.size());
    for (int z = 0; z < dimension; z++) {
        for (int x = 0; x < dimension; x++) {
            colors[z * dimension + x] = calcColor(heights[z * dimension + x], amplitude);
        }
    }
    return colors;
}

glm::vec4 ColorGenerator::getRandomColor() {
    std::uniform_real_distribution<> dis(0.0, 1.0);
    return glm::vec4(getRandF(), getRandF(), getRandF(), 1.0f);
}

void ColorGenerator::initRandom() {
    randEng_ = std::default_random_engine{static_cast<unsigned int>(time(0))};
}

float ColorGenerator::getRandF() {
    std::uniform_real_distribution<> dis(0.0, 1.0);
    return dis(randEng_);
}

glm::vec4 ColorGenerator::calcColor(float height, float amplitude) {
    float value = (height + amplitude) / (amplitude * 2);
    value = glm::clamp((value - halfSpread_) * (1.0f / spread_), 0.0f, 0.9999f);
    int firstPalette = glm::floor(value / part_);
    float blend = (value - (firstPalette * part_)) / part_;
    return interpolateColors(colorPalette_[firstPalette], colorPalette_[firstPalette + 1], blend);
}

glm::vec4 ColorGenerator::interpolateColors(glm::vec4 col1, glm::vec4 col2, float blend) {
    float color1weight = 1 - blend;
    float r = (color1weight * col1.x) + (blend * col2.x);
    float g = (color1weight * col1.y) + (blend * col2.y);
    float b = (color1weight * col1.z) + (blend * col2.z);

    return glm::vec4(r / 255.0f, g / 255.0f, b / 255.0f, 1.0f);
}