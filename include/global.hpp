#ifndef GLOBAL_H
#define GLOBAL_H

#include "threadpool.hpp"

extern float g_deltaTime;
extern float g_currentFrameTime;
extern unsigned int g_triangleCount;
extern bool g_debugPolygonMode;

extern ThreadPool *threadPool;

#endif