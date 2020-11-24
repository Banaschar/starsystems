#include "engine.hpp"
#include "oglheader.hpp"
#include "windowheader.hpp"

float g_deltaTime = 0.0f;
float g_currentFrameTime;
unsigned int g_triangleCount = 0;
bool g_debugPolygonMode = false;
ThreadPool *g_threadPool = nullptr;

Engine::Engine(int width, int height, const std::string &name) {
    initWindow(width, height, name);
    initThreadPool();
}

Engine::~Engine() {
    if (scene_)
        delete scene_;

    if (g_threadPool)
        delete g_threadPool;
}

void Engine::initWindow(int width, int height, const std::string &name) {
    glewExperimental = true;
    if (!glfwInit()) {
        fprintf(stderr, "Failed to init GLFW\n");
        return;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open windows and create its opengl context
    window_ = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);
    if (window_ == nullptr) {
        fprintf(stderr, "Failed to open GLFW window\n");
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window_);

    // Init GLEW
    GLenum glewinit = glewInit();
    if (glewinit != GLEW_OK) {
        fprintf(stdout, "GlEW init failed! Error: %s\n", glewGetErrorString(glewinit));
        glfwTerminate();
        return;
    }

    GLint maxPatchVert = 0;
    glGetIntegerv(GL_MAX_PATCH_VERTICES, &maxPatchVert);
    fprintf(stdout, "[ENGINE::initWindow] MaxPatchVertices: %i\n", maxPatchVert);
    glPatchParameteri(GL_PATCH_VERTICES, 3);

    // Init key capture
    glfwSetInputMode(window_, GLFW_STICKY_KEYS, GL_TRUE);
    // Limit mouse movement
    glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Background color
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    // Only draw if new fragment is closer to camera then one behind
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    // Don't render triangles which normal is not torwards the camera
    glEnable(GL_CULL_FACE);
}

void Engine::initThreadPool() {
    int numThreads = std::thread::hardware_concurrency();
    g_threadPool = new ThreadPool(numThreads == 0 ? 4 : numThreads);
    fprintf(stdout, "[ENGINE::initThreadPool]: ThreadPool created with %i threads.\n", numThreads);
}

GLFWwindow *Engine::getWindow() {
    return window_;
}

void Engine::addScene(Scene *scene) {
    scene_ = scene;
}

void Engine::render() {
    if (scene_)
        render_();
    else
        fprintf(stderr, "No scene to render\n");
}

void Engine::render_() {
    int nbFrames = 0;
    float lastFrame = glfwGetTime();
    float lastTime = lastFrame;
    do {
        g_currentFrameTime = glfwGetTime();
        g_deltaTime = g_currentFrameTime - lastFrame;
        lastFrame = g_currentFrameTime;
        nbFrames++;
        if (g_currentFrameTime - lastTime >= 1.0) {
            fprintf(stdout, "%f ms/frame -> %i FPS. Triangles per Frame: %u\n", 1000.0 / float(nbFrames), nbFrames, g_triangleCount/nbFrames);
            nbFrames = 0;
            lastTime += 1.0;
            g_triangleCount = 0;
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        scene_->update();
        scene_->render();

        glfwSwapBuffers(window_);
        glfwPollEvents();
    } while (glfwGetKey(window_, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window_) == 0);

    glfwTerminate();
}