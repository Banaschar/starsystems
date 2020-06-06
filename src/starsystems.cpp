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

    // Open windows and create its opengl context
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
    Shader shader = Shader("StandardShading.vertexshader", "StandardShading.fragmentshader");

    // Get handle for mvp uniform
    //GLuint matrixID = glGetUniformLocation(programID, "MVP");
    //GLuint cameraMatrixID = glGetUniformLocation(programID, "V");
    //GLuint modelMatrixID = glGetUniformLocation(programID, "M");

    GLuint Texture = loadDds("uvmap.DDS");
    //GLuint TextureID  = glGetUniformLocation(programID, "myTextureSampler");

    View view = View(window, glm::vec3(0,0,-6));

    Model model1 = Model("suzanne.obj");

    Scene scene = Scene(view, shader);
    scene.addModel(model1);

    //glm::mat4 modelMatrix = glm::mat4(1.0);
    //glm::mat4 MVP = view.getProjectionMatrix() * view.getCameraMatrix() * modelMatrix;

    //glUseProgram(programID);
    //GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");

    do {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        /*
        shader.use();
        view.rotateCamera();
        MVP = view.getProjectionMatrix() * view.getCameraMatrix() * modelMatrix;
        shader.uniform("MVP", MVP);
        shader.uniform("M", modelMatrix);
        shader.uniform("V", view.getCameraMatrix());

        glm::vec3 lightPos = glm::vec3(4,4,4);
        shader.uniform("LightPosition_worldspace", lightPos);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Texture);
        shader.uniform("myTextureSampler", 0);
        model1.draw();
        */

        scene.update();
        scene.render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    } while(glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
            glfwWindowShouldClose(window) == 0);

    glfwTerminate();

    return 0;
}