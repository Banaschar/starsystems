#ifndef GLOBAL_H
#define GLOBAL_H

#include "threadpool.hpp"

#define SHADER_TYPE_LIGHT "default_light"
#define SHADER_TYPE_TERRAIN "default_terrain"
#define SHADER_TYPE_SKY "default_sky"
#define SHADER_TYPE_WATER "default_water"
#define SHADER_TYPE_WATER_PERFORMANCE "default_water_performance"
#define SHADER_TYPE_GUI "default_gui"

extern float g_deltaTime;
extern unsigned int g_triangleCount;

extern ThreadPool *threadPool;

#endif