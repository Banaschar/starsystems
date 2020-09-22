#include "perlinnoise.hpp"
#include <cstdio>
#include <glm/glm.hpp>
#include <random>

PerlinNoise::PerlinNoise() : PerlinNoise(DEFAULT_OCTAVES, DEFAULT_AMPLITUDE, DEFAULT_ROUGHNESS, DEFAULT_HEIGHT_OFFSET_FACTOR) {}
PerlinNoise::PerlinNoise(int octaves, float amplitude, float roughness, int heightOffsetFactor, unsigned int seed)
    : octaves_(octaves), amplitude_(amplitude), roughness_(roughness), seed_(seed) {
    if (seed_)
        p = generateRandomPerm();
    else
        p = perm_;

    heightOffset_ = heightOffsetFactor == 0 ? 0 : (amplitude_ / heightOffsetFactor);
}

/*
 * Function based on "Fractional Brownian motion"
 * to have multiple parameters to vary the created terrain.
 * Also handles the problem with integers in perlin.
 *
 * Other ideas: Increasing frequency. Decreasing amplitude
 */
float PerlinNoise::getNoise2d(int x, int y) {
    float total = 0;
    float d = (float)glm::pow(2, octaves_ - 1);
    for (int i = 0; i < octaves_; i++) {
        float freq = (float)(glm::pow(2, i) / d);
        float amp = (float)glm::pow(roughness_, i) * amplitude_;
        total += noise2d(x * freq, y * freq) * amp;
    }
    return total + heightOffset_;
}

float PerlinNoise::getNoise3d(int x, int y, int z) {
    float total = 0;
    float d = (float)glm::pow(2, octaves_ - 1);
    for (int i = 0; i < octaves_; i++) {
        float freq = (float)(glm::pow(2, i) / d);
        float amp = (float)glm::pow(roughness_, i) * amplitude_;
        total += noise3d(x * freq, y * freq, z * freq) * amp;
    }
    return total + heightOffset_;
}

float PerlinNoise::getAmplitude() {
    return amplitude_;
}

std::vector<int> PerlinNoise::generateRandomPerm() {
    fprintf(stdout, "[PerlinNoise] Generating random permutation Vector. Seed: %u\n", seed_);

    std::default_random_engine eng{seed_};
    std::uniform_int_distribution<> distrib(0, 255);

    std::vector<int> perms(512);
    for (int i = 0; i < 256; i++) {
        perms[256 + i] = perms[i] = distrib(eng);
    }

    return perms;
}
float PerlinNoise::noise2d(float x, float y) {
    int ix0, iy0, ix1, iy1;
    float fx0, fy0, fx1, fy1;
    float s, t, nx0, nx1, n0, n1;

    ix0 = (int)glm::floor(x);
    iy0 = (int)glm::floor(y);
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

float PerlinNoise::noise3d(float x, float y, float z) {
    int ix0, iy0, ix1, iy1, iz0, iz1;
    float fx0, fy0, fz0, fx1, fy1, fz1;
    float s, t, r;
    float nxy0, nxy1, nx0, nx1, n0, n1;

    ix0 = (int)glm::floor(x);
    iy0 = (int)glm::floor(y);
    iz0 = (int)glm::floor(z);
    fx0 = x - ix0;
    fy0 = y - iy0;
    fz0 = z - iz0;
    fx1 = fx0 - 1.0f;
    fy1 = fy0 - 1.0f;
    fz1 = fz0 - 1.0f;
    ix1 = ( ix0 + 1 ) & 0xff;
    iy1 = ( iy0 + 1 ) & 0xff;
    iz1 = ( iz0 + 1 ) & 0xff;
    ix0 = ix0 & 0xff;
    iy0 = iy0 & 0xff;
    iz0 = iz0 & 0xff;
    
    r = fade( fz0 );
    t = fade( fy0 );
    s = fade( fx0 );

    nxy0 = grad(p[ix0 + p[iy0 + p[iz0]]], fx0, fy0, fz0);
    nxy1 = grad(p[ix0 + p[iy0 + p[iz1]]], fx0, fy0, fz1);
    nx0 = lerp(r, nxy0, nxy1);

    nxy0 = grad(p[ix0 + p[iy1 + p[iz0]]], fx0, fy1, fz0);
    nxy1 = grad(p[ix0 + p[iy1 + p[iz1]]], fx0, fy1, fz1);
    nx1 = lerp(r, nxy0, nxy1);

    n0 = lerp(t, nx0, nx1);

    nxy0 = grad(p[ix1 + p[iy0 + p[iz0]]], fx1, fy0, fz0);
    nxy1 = grad(p[ix1 + p[iy0 + p[iz1]]], fx1, fy0, fz1);
    nx0 = lerp(r, nxy0, nxy1);

    nxy0 = grad(p[ix1 + p[iy1 + p[iz0]]], fx1, fy1, fz0);
    nxy1 = grad(p[ix1 + p[iy1 + p[iz1]]], fx1, fy1, fz1);
    nx1 = lerp(r, nxy0, nxy1);

    n1 = lerp(t, nx0, nx1);
    
    return 0.936f * (lerp(s, n0, n1));
}

float PerlinNoise::fade(float t) {
    return t * t * t * (t * (t * 6 - 15) + 10);
}

float PerlinNoise::lerp(float t, float a, float b) {
    return a + t * (b - a);
}

float PerlinNoise::grad(int hash, float x, float y) {
    int h = hash & 7;
    float u = h < 4 ? x : y;
    float v = h < 4 ? y : x;

    return ((h & 1) ? -u : u) + ((h & 2) ? -2.0 * v : 2.0 * v);
}

float PerlinNoise::grad(int hash, float x, float y, float z) {
    int h = hash & 15;     
    float u = h<8 ? x : y;
    float v = h<4 ? y : h==12||h==14 ? x : z;
    return ((h&1)? -u : u) + ((h&2)? -v : v);
}