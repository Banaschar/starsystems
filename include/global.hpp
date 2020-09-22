#ifndef GLOBAL_H
#define GLOBAL_H

#include "threadpool.hpp"

enum class ShaderType {SHADER_TYPE_DEFAULT, SHADER_TYPE_LIGHT, SHADER_TYPE_TERRAIN, SHADER_TYPE_SKY, SHADER_TYPE_WATER, SHADER_TYPE_WATER_PERFORMANCE,
                        SHADER_TYPE_GUI, SHADER_TYPE_POST_PROCESSOR, SHADER_TYPE_DEBUG};


extern float g_deltaTime;
extern unsigned int g_triangleCount;
extern bool g_debugPolygonMode;

extern ThreadPool *threadPool;

#endif