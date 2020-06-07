#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <forward_list>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/string_cast.hpp>
using namespace glm;

#include "shader.hpp"
#include "view.hpp"
#include "mesh.hpp"
#include "model.hpp"
#include "scene.hpp"
#include "assetloader.hpp"
#include "global.hpp"
#include "planet.hpp"
#include "stargame.hpp"

float deltaTime = 0.0f;

int main() 
{
    glewExperimental = true;
    if (!glfwInit()) {
        fprintf(stderr, "Failed to init GLFW\n");
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //Open windows and create its opengl context
    GLFWwindow* window;
    window = glfwCreateWindow(1280, 720, "star systems", NULL, NULL);
    if (window == NULL) {
        fprintf(stderr, "Failed to open GLFW window\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Init GLEW
    GLenum glewinit = glewInit();
    if (glewinit != GLEW_OK) {
        std::cout << "GlEW init failed! " << glewGetErrorString(glewinit);
        glfwTerminate();
        return -1;
    }

    // Init key capture
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // Background color
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

    // Only draw if new fragment is closer to camera then one behind
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    // Don't render triangles which normal is not torwards the camera
    glEnable(GL_CULL_FACE);

    // Create and compile shaders
    Shader shaderPlanet = Shader("StandardShading.vs", "StandardShading.fs", "planet");
    Shader shaderSun = Shader("sun.vs", "sun.fs", "light");

    GLuint Texture = loadDds("uvmap.DDS");
    //GLuint TextureID  = glGetUniformLocation(programID, "myTextureSampler");

    View view = View(window, glm::vec3(0,12,-60));

    Model sun = Model("PlanetFirstTry.obj", shaderSun);

    Scene scene = Scene(view, sun);

    // Add planets
    glm::vec3 trans;
    glm::vec3 scale = glm::vec3(0.5f, 0.5f, 0.5f);
    Model tmp1 = Model("PlanetFirstTry.obj", shaderPlanet);
    trans = glm::vec3(10, 0, 0);
    tmp1.transform(&scale, &trans, NULL);
    
    Model tmp2 = Model("PlanetFirstTry.obj", shaderPlanet);
    trans = glm::vec3(20, 0, 20);
    tmp2.transform(&scale, &trans, NULL);

    
    Model tmp3 = Model("PlanetFirstTry.obj", shaderPlanet);
    trans = glm::vec3(30, 0, 40);
    tmp3.transform(&scale, &trans, NULL);

    Planet planet = Planet(tmp1, 0.7f);
    Planet planet2 = Planet(tmp2, 0.5f);
    Planet planet3 = Planet(tmp3, 0.2f);
    scene.addModel(&planet);
    scene.addModel(&planet2);
    scene.addModel(&planet3);

    //scene.setAutoRotate(true);

    int nbFrames = 0;
    float lastFrame = glfwGetTime();
    float lastTime = lastFrame;
    do {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        nbFrames++;
        if (currentFrame - lastTime >= 1.0) {
            printf("%f ms/frame -> %i FPS\n", 1000.0/float(nbFrames), nbFrames);
            nbFrames = 0;
            lastTime += 1.0;
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        scene.update();
        scene.render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    } while(glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
            glfwWindowShouldClose(window) == 0);

    glfwTerminate();

    return 0;
}