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

/*
 * Based on the improved Perlin Noise
 * https://mrl.nyu.edu/~perlin/noise/
 *
 * With basic fractional brownian motion 
 */
class PerlinNoise {
public:
    PerlinNoise() : PerlinNoise(3, 4, 0.35) {}
    PerlinNoise(int octaves, float amplitude, float roughness, unsigned int seed = 0) :
        octaves_(octaves), amplitude_(amplitude), roughness_(roughness), seed_(seed) {
            if (seed_)
                p = generateRandomPerm();
            else
                p = perm_;
    }

    float getNoise2d(int x, int y) {
        float total = 0;
        float d = (float) glm::pow(2, octaves_ - 1);
        for (int i = 0; i < octaves_; i++) {
            float freq = (float) (glm::pow(2, i) / d);
            float amp = (float) glm::pow(roughness_, i) * amplitude_;
            total += noise2d(x * freq, y * freq) * amp;
        }
        return total;
    }

    float getAmplitude() {
        return amplitude_;
    }
private:
    int octaves_;
    float amplitude_;
    float roughness_;
    unsigned int seed_;

    std::vector<int> generateRandomPerm() {
        std::cout << "[PerlinNoise] Generating random permutation Vector. Seed: " << seed_ << std::endl;
        
        std::default_random_engine eng{seed_};
        std::uniform_int_distribution<> distrib(0, 255);
        
        std::vector<int> perms(512);
        for (int i = 0; i < 256; i++) {
            perms[256 + i] = perms[i] = distrib(eng);
        }

        return perms;
    }
    float noise2d(float x, float y) {
        int ix0, iy0, ix1, iy1;
        float fx0, fy0, fx1, fy1;
        float s, t, nx0, nx1, n0, n1;

        ix0 = (int) glm::floor(x);
        iy0 = (int) glm::floor(y);
        fx0 = x - ix0;
        fy0 = y - iy0;
        fx1 = fx0 - 1.0f;
        fy1 = fy0 - 1.0f;
        ix1 = (ix0 + 1) & 0xff;
        iy1 = (iy0 + 1) & 0xff;
        ix0 = ix0 & 0xff;
        iy0 = iy0 & 0xff;
        
        t = fade(fy0);
        s = fade(fx0);

        nx0 = grad(p[ix0 + p[iy0]], fx0, fy0);
        nx1 = grad(p[ix0 + p[iy1]], fx0, fy1);
        n0 = lerp(t, nx0, nx1);

        nx0 = grad(p[ix1 + p[iy0]], fx1, fy0);
        nx1 = grad(p[ix1 + p[iy1]], fx1, fy1);
        n1 = lerp(t, nx0, nx1);
        return 0.507f * (lerp(s, n0, n1));
    }

    float fade(float t) {
        return t * t * t * (t * (t * 6 - 15) + 10);
    }

    float lerp(float t, float a, float b) {
        return a + t * (b - a);
    }

    float grad(int hash, float x, float y) {
        int h = hash & 7;
        float u = h < 4 ? x : y;
        float v = h < 4 ? y : x;

        return ((h & 1) ? -u : u) + ((h & 2) ? -2.0*v : 2.0*v);
    }
    std::vector<int> p;
    std::vector<int> perm_ = {
        151,160,137,91,90,15, // permutation vector
       131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
       190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
       88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
       77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
       102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
       135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
       5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
       223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
       129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
       251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
       49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
       138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180,
       151,160,137,91,90,15, // 2mal das selbe
       131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
       190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
       88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
       77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
       102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
       135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
       5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
       223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
       129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
       251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
       49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
       138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
    };
};

/*
class PerlinNoise {
public:
    PerlinNoise(int octaves, float amplitude, float roughness) :
        octaves_(octaves), amplitude_(amplitude), roughness_(roughness) {
            seed_ = getRandomInt(1000000000);
            std::cout << "random seed: " << seed_ << std::endl;
        }
    PerlinNoise(int seed, int octaves, float amplitude, float roughness) :
        seed_(seed), octaves_(octaves), amplitude_(amplitude), roughness_(roughness) {
            
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
        float ran = dis(eng);
        return ran;
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
        float f = (float) ((1.0f - glm::cos(theta)) * 0.5f);
        return a * (1 - f) + b * f;
    }
};
*/

class ColorGenerator {
public:
    ColorGenerator() {
        initRandom();
        spread_ = getRandF();
        for (int i = 0; i < 5; i++) {
            colorPalette_.push_back(getRandomColor());
        }
    }
    ColorGenerator(std::vector<glm::vec4> colorPalette, float spread) :
                colorPalette_(colorPalette), spread_(spread) {
        halfSpread_ = spread_ / 2.0f;
        part_ = 1.0f / (colorPalette_.size() - 1);
        initRandom();
    }

    std::vector<glm::vec4> genColors(std::vector<float> &heights, 
                                        float dimension, 
                                        float amplitude = 1.0f) {
        std::vector<glm::vec4> colors (heights.size());
        for (int z = 0; z < dimension; z++) {
            for(int x = 0; x < dimension; x++) {
                colors[z * dimension + x] = calcColor(heights[z * dimension + x], amplitude);
            }
        }
        return colors;
    }

    glm::vec4 getRandomColor() {
        std::uniform_real_distribution<> dis(0.0, 1.0);
        return glm::vec4(getRandF(), getRandF(), getRandF(), 1.0f);
    }
private:
    std::vector<glm::vec4> colorPalette_;
    float spread_;
    float halfSpread_;
    float part_;
    std::default_random_engine randEng_;

    void initRandom() {
         randEng_ = std::default_random_engine{static_cast<unsigned int>(time(0))};
    }

    float getRandF() {
        std::uniform_real_distribution<> dis(0.0, 1.0);
        return dis(randEng_);
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

class TerrainGenerator {
public:
    TerrainGenerator(ColorGenerator colorGen, PerlinNoise pNoise) : colorGen_(colorGen), pNoise_(pNoise) {};

    Mesh generateTerrain(int dimension) {
        std::vector<float> heights = generateHeights(dimension, pNoise_);
        std::vector<glm::vec4> colors = colorGen_.genColors(heights, dimension, pNoise_.getAmplitude());

        return generateMesh(heights, colors, dimension);
    }

private:
    ColorGenerator colorGen_;
    PerlinNoise pNoise_;

    float getHeightN(int x, int z, std::vector<float> &heights, int dim) {
        x = x < 0 ? 0 : x;
        z = z < 0 ? 0 : z;
        x = x >= dim ? dim - 1 : x;
        z = z >= dim ? dim - 1 : z;
        return heights[z * dim + x];
    }

    glm::vec3 calcNormal(int x, int z, std::vector<float> &heights, int dim) {
        float heightL = getHeightN(x - 1, z, heights, dim);
        float heightR = getHeightN(x + 1, z, heights, dim);
        float heightD = getHeightN(x, z - 1, heights, dim);
        float heightU = getHeightN(x, z + 1, heights, dim);

        return glm::normalize(glm::vec3(heightL - heightR, 2.0f, heightD - heightU));
    }

    /*
     * Switch out for the normal generator in primitives
     *
     */
    std::vector<glm::vec3> generateNormalVector(std::vector<float> &heights, int dimension) {
        std::vector<glm::vec3> normals(dimension * dimension);
        for (int z = 0; z < dimension; z++) {
            for (int x = 0; x < dimension; x++) {
                normals[z * dimension + x] = calcNormal(x, z, heights, dimension);
            }
        }
        return normals;
    }

    std::vector<float> generateHeights(int dimension, PerlinNoise pNoise) {
        std::vector<float> heights(dimension * dimension);
        for (int z = 0; z < dimension; z++) {
            for (int x = 0; x < dimension; x++) {
                heights[z * dimension + x] = pNoise.getNoise2d(x, z);
            }
        }
        return heights;
    }

    std::vector<unsigned int> generateIndexVector(int dimension) {
        std::vector<unsigned int> indices ((dimension-1) * (dimension-1) * 6);
        int cnt = 0;
        for (int row = 0; row < dimension - 1; row++) {
            for (int col = 0; col < dimension - 1; col++) {
                indices[cnt++] = dimension * row + col;
                indices[cnt++] = dimension * row + col + dimension;
                indices[cnt++] = dimension * row + col + dimension + 1;

                indices[cnt++] = dimension * row + col;
                indices[cnt++] = dimension * row + col + dimension + 1;
                indices[cnt++] = dimension * row + col + 1;
            }
        }

        return indices;
    }

    Mesh generateMesh(std::vector<float> heights, std::vector<glm::vec4> colors, int dimension) {
        std::vector<Vertex> vertices (dimension * dimension);
        std::vector<glm::vec3> normals = generateNormalVector(heights, dimension);
        int half = dimension / 2;
        for (int z = 0; z < dimension; z++) {
            for (int x = 0; x < dimension; x++) {
                Vertex vertex;
                vertex.position.x = x - half;
                vertex.position.y = heights[z * dimension + x];
                vertex.position.z = z - half;
                vertex.normal = normals[z * dimension + x];
                vertex.color = colors[z * dimension + x];
                vertices[z * dimension + x] = vertex;
            }
        }
        return Mesh(vertices, generateIndexVector(dimension));
    }
};

#endif