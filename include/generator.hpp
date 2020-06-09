#ifndef GENERATOR_H
#define GENERATOR_H

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <random>
#include <ctime>
#include <bits/stdc++.h>

#include "mesh.hpp"
#include "primitives.hpp"

class PerlinNoise {
public:
    PerlinNoise(int octaves, float amplitude, float roughness) :
        octaves_(octaves), amplitude_(amplitude), roughness_(roughness) {
            seed_ = getRandomInt(1000000000);
        }
    PerlinNoise(int seed, int octaves, float amplitude, float roughness) :
        seed_(seed), octaves_(octaves), amplitude_(amplitude), roughness_(roughness) {
            
    }

    int getRandomInt() {
        std::srand((unsigned) time(0));
        return std::rand();
    }

    float getPerlinNoise(int x, int y) {
        float total = 0;
        float d = (float) glm::pow(2, octaves_ - 1);
        for (int i = 0; i < octaves_; i++) {
            float freq = (float) (glm::pow(2, i) / d);
            float amp = (float) glm::pow(roughness_, i) * amplitude_;
            total += getInterpolatedNoise(x * freq, y * freq) * amp;
        }
        return total;
    }

    int getSeed() {
        return seed_;
    }

    float getAmplitude() {
        return amplitude_;
    }

private:
    int seed_;
    int octaves_;
    float amplitude_;
    float roughness_;
    int getRandomInt(int max) {
        std::default_random_engine eng{static_cast<unsigned int>(time(0))};
        std::uniform_int_distribution<> distrib(0, INT_MAX);
        return distrib(eng);
    }

    float getRandomFloat(unsigned int seed) {
        std::default_random_engine eng{seed};
        std::uniform_real_distribution<> dis(0.0, 1.0);
        return dis(eng);
    }

    float getNoise(int x, int y) {
        return getRandomFloat(x * 49632 + y * 325176 + seed_) * 2.0f - 1.0f;
    }

    float getSmoothNoise(int x, int y) {
        float corners = (getNoise(x - 1, y - 1) + getNoise(x + 1, y - 1) + getNoise(x - 1, y + 1) +
                            getNoise(x + 1, y + 1)) / 16.0f;
        float sides = (getNoise(x - 1, y) + getNoise(x + 1, y) + getNoise(x, y - 1) + getNoise(x, y + 1)) / 8.0f;
        float center = getNoise(x, y) / 4.0f;
        return corners + sides + center;
    }

    float getInterpolatedNoise(float x, float y) {
        int aX = (int) x;
        int aY = (int) y;
        float fracX = x - aX;
        float fracY = y - aY;

        float v1 = getSmoothNoise(aX, aY);
        float v2 = getSmoothNoise(aX + 1, aY);
        float v3 = getSmoothNoise(aX, aY + 1);
        float v4 = getSmoothNoise(aX + 1, aY + 1);
        float i1 = interpolate(v1, v2, fracX);
        float i2 = interpolate(v3, v4, fracX);

        return interpolate(i1, i2, fracY);
    }

    float interpolate(float a, float b, float blend) {
        double theta = blend * glm::pi<float>();
        float f = (float) ((1.0f - glm::cos(theta)) + 0.5f);
        return a * (1 - f) + b * f;
    }
};

class ColorGen {
private:
    std::vector<glm::vec4> colorPalette_;
    float spread_;
    float halfSpread_;
    float part_;

public:
    ColorGen(std::vector<glm::vec4> colorPalette, float spread) :
                colorPalette_(colorPalette), spread_(spread) {
        halfSpread_ = spread_ / 2.0f;
        part_ = 1.0f / (colorPalette_.size() - 1);
    }

    std::vector<glm::vec4> genColors(std::vector<float> &heights, 
                                        float dimension, 
                                        float amplitude) {
        std::vector<glm::vec4> colors (heights.size());
        for (int z = 0; z < dimension; z++) {
            for(int x = 0; x < dimension; x++) {
                colors[z * dimension + x] = calcColor(heights[z * dimension + x], amplitude);
            }
        }
        return colors;
    }

    glm::vec4 calcColor(float height, float amplitude) {
        float value = (height + amplitude) / (amplitude * 2);
        value = glm::clamp((value - halfSpread_) * (1.0f / spread_), 0.0f, 0.9999f);
        int firstPalette = glm::floor(value / part_);
        float blend = (value - (firstPalette * part_)) / part_;
        return interpolateColors(colorPalette_[firstPalette], colorPalette_[firstPalette + 1], blend);
    }

    glm::vec4 interpolateColors(glm::vec4 col1, glm::vec4 col2, float blend) {
        float color1weight = 1 - blend;
        float r = (color1weight * col1.x) + (blend * col2.x);
        float g = (color1weight * col1.y) + (blend * col2.y);
        float b = (color1weight * col1.z) + (blend * col2.z);

        return glm::vec4(r/255.0f,g/255.0f,b/255.0f,1.0f);
    }
};

std::vector<float> generateHeights(int dimension, PerlinNoise pNoise) {
    std::vector<float> heights(dimension*dimension);
    for (int z = 0; z < dimension; z++) {
        for (int x = 0; x < dimension; x++) {
            heights[z * dimension + x] = pNoise.getPerlinNoise(x, z);
        }
    }
    return heights;
}

Mesh createLandscape() {
    int dimension = 100;
    Mesh mesh = createPlane(dimension);
    std::vector<Vertex> &vertices = mesh.getVertices(); 

    std::vector<glm::vec4> colorPalette = {
        glm::vec4(201, 178, 99, 1),
        glm::vec4(135, 184, 82, 1),
        glm::vec4(80, 171, 93, 1),
        glm::vec4(120, 120, 120, 1),
        glm::vec4(200, 200, 210, 1)
    };
    PerlinNoise pNoise = PerlinNoise(3, 10, 0.35f);
    ColorGen colorGen = ColorGen(colorPalette, 0.45f);
    std::vector<float> heights = generateHeights(dimension, pNoise);
    std::vector<glm::vec4> colors = colorGen.genColors(heights, dimension, pNoise.getAmplitude());

    for (int i = 0; i < vertices.size(); i++) {
        vertices[i].position.y = heights[i];
        vertices[i].color = colors[i];
        //std::cout << "color: " << glm::to_string(colors[i]) << std::endl;
    }

    mesh.updateMesh();
    
    return mesh;
}

#endif