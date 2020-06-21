#include "engine.hpp"
#include "oglheader.hpp"
#include "windowheader.hpp"

float g_deltaTime = 0.0f;

Engine::Engine(int width, int height, const std::string name) {
    initWindow(width, height, name);
}

Engine::~Engine() {
    if (scene_)
        delete scene_;
}

void Engine::initWindow(int width, int height, const std::string name) {
    glewExperimental = true;
    if (!glfwInit()) {
        fprintf(stderr, "Failed to init GLFW\n");
        return;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //Open windows and create its opengl context
    window_ = glfwCreateWindow(1280, 720, "star systems", NULL, NULL);
    if (window_ == NULL) {
        fprintf(stderr, "Failed to open GLFW window\n");
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window_);

    // Init GLEW
    GLenum glewinit = glewInit();
    if (glewinit != GLEW_OK) {
        std::cout << "GlEW init failed! " << glewGetErrorString(glewinit);
        glfwTerminate();
        return;
    }

    // Init key capture
    glfwSetInputMode(window_, GLFW_STICKY_KEYS, GL_TRUE);

    // Background color
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

    // Only draw if new fragment is closer to camera then one behind
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    // Don't render triangles which normal is not torwards the camera
    glEnable(GL_CULL_FACE);
}

GLFWwindow* Engine::getWindow() {
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
        float currentFrame = glfwGetTime();
        g_deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        nbFrames++;
        if (currentFrame - lastTime >= 1.0) {
            fprintf(stdout, "%f ms/frame -> %i FPS\n", 1000.0/float(nbFrames), nbFrames);
            nbFrames = 0;
            lastTime += 1.0;
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        scene_->update();
        scene_->render();

        glfwSwapBuffers(window_);
        glfwPollEvents();
    } while(glfwGetKey(window_, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
            glfwWindowShouldClose(window_) == 0);

    glfwTerminate();
}