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
#include "game.hpp"
#include "primitives.hpp"
#include "generator.hpp"

float deltaTime = 0.0f;

/*
Architecture:
Create a game class -> game object holds all the initial initialization (and memory!)
-> Can switch out game objects to test different things (e.g. follow the tutorials)
--> Need to move the shader options (uniforms etc.) out of the model class.
--> Or somehow initialize it with the necessary function calls.
-----> Maybe have a callback function in Model, that gets registered during initialization 

*/

void standardShadingCb(Shader *shader, Drawable *drawable, Game *game) {
    Model *model = dynamic_cast<Model*>(drawable);

    shader->use();
    shader->uniform("MVP", model->getMvp());
    shader->uniform("lightPosition", game->getLightSource()->getPosition());
    shader->uniform("modelMatrix", model->getModelMatrix());
    shader->uniform("normalMatrix", model->getNormalMatrix());
    shader->uniform("cameraMatrix", game->getView().getCameraMatrix());

    //shader_.uniform("objectColor", glm::vec3(1.0f, 0.5f, 0.31f));
    shader->uniform("material.ambient", glm::vec3(1.0f, 0.5f, 0.31f));
    shader->uniform("material.diffuse", glm::vec3(1.0f, 0.5f, 0.31f));
    shader->uniform("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
    shader->uniform("material.shininess", 32.0f);
    shader->uniform("light.ambient", glm::vec3(0.1f, 0.1f, 0.1f));
    shader->uniform("light.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));  
    shader->uniform("light.specular", glm::vec3(1.0f, 1.0f, 1.0f)); 
}

void sunShaderCb(Shader *shader, Drawable *drawable, Game *game) {
    Model *model = dynamic_cast<Model*>(drawable);

    shader->use();
    shader->uniform("MVP", model->getMvp());
    shader->uniform("color", glm::vec4(1.0f, 1.0f, 0.2f, 0.7f));
}

void skyBoxShaderCb(Shader *shader, Drawable *drawable, Game *game) {
    Model *model = dynamic_cast<Model*>(drawable);
    
    shader->use();
    // remove translation, so skybox is not changed by moving around
    glm::mat4 mvp = game->getView().getProjectionMatrix() *
        glm::mat4(glm::mat3(game->getView().getCameraMatrix())) * 
        model->getModelMatrix();
    
    shader->uniform("MVP", mvp);
}

Scene* createStarSystems(GLFWwindow* window) {
    std::vector<std::string> cubetex = {
        "skyboxSpace/right.jpg",
        "skyboxSpace/left.jpg",
        "skyboxSpace/top.jpg",
        "skyboxSpace/bottom.jpg",
        "skyboxSpace/front.jpg",
        "skyboxSpace/back.jpg"
    };
    Texture cubemapTexture = loadCubeMap(cubetex);
    // Create and compile shaders
    std::vector<Shader*> shaders = { 
        new Shader("StandardShading.vs", "StandardShading.fs", 
                                        "planet", standardShadingCb),
        new Shader("sun.vs", "sun.fs", "light", sunShaderCb),
        new Shader("skybox.vs", "skybox.fs", "skybox", skyBoxShaderCb)
    };

    View view = View(window, glm::vec3(0,12,-60));

    // Sun
    Model *sun = new Model("PlanetFirstTry.obj", "light");

    // Skybox
    Mesh box = createCube(1);
    box.addTexture(cubemapTexture);
    Model *skybox = new Model(box, "skybox");
    
    // Add planets
    glm::vec3 trans;
    glm::vec3 scale = glm::vec3(0.5f, 0.5f, 0.5f);
    Model tmp1 = Model("PlanetFirstTry.obj", "planet");
    trans = glm::vec3(10, 0, 0);
    tmp1.transform(&scale, &trans, NULL);
    
    Model tmp2 = Model("PlanetFirstTry.obj", "planet");
    trans = glm::vec3(20, 0, 20);
    tmp2.transform(&scale, &trans, NULL);

    
    Model tmp3 = Model("PlanetFirstTry.obj", "planet");
    trans = glm::vec3(30, 0, 40);
    tmp3.transform(&scale, &trans, NULL);

    Planet *planet = new Planet(tmp1, 0.7f);
    Planet *planet2 = new Planet(tmp2, 0.5f);
    Planet *planet3 = new Planet(tmp3, 0.2f);

    Game *game = new Game(view);
    game->addLight(sun);
    game->addModel(skybox);
    game->addModel(planet);
    game->addModel(planet2);
    game->addModel(planet3);

    Scene *scene = new Scene(game, shaders);

    return scene;
}

void planeShaderCb(Shader *shader, Drawable *drawable, Game *game) {
    Model *model = dynamic_cast<Model*>(drawable);
    
    shader->use();
    shader->uniform("MVP", model->getMvp());
    shader->uniform("normalMatrix", model->getNormalMatrix());
    shader->uniform("light.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
    shader->uniform("light.ambient", glm::vec3(1.0, 1.0, 1.0));
    shader->uniform("light.diffuse", glm::vec3(1.0, 1.0, 1.0));
}

void flatColorCb(Shader *shader, Drawable *drawable, Game *game) {
    Model *model = dynamic_cast<Model*>(drawable);
    
    shader->use();
    shader->uniform("MVP", model->getMvp());
}

void instanceShaderCb(Shader *shader, Drawable *drawable, Game *game) {
    shader->use();
    shader->uniform("projection", game->getView().getProjectionMatrix());
    shader->uniform("camera", game->getView().getCameraMatrix());
}

void waterShaderCb(Shader *shader, Drawable *drawable, Game *game) {
    Model *model = dynamic_cast<Model*>(drawable);

    shader->use();
    shader->uniform("MVP", model->getMvp());
    shader->uniform("normalMatrix", model->getNormalMatrix());
    shader->uniform("light.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
    shader->uniform("light.ambient", glm::vec3(1.0, 1.0, 1.0));
    shader->uniform("light.diffuse", glm::vec3(1.0, 1.0, 1.0));
}

Scene* createPlane(GLFWwindow *window) {
    std::cout << "Creat Plane" << std::endl;

    std::vector<std::string> cubetex = {
        "skyboxSky/right.jpg",
        "skyboxSky/left.jpg",
        "skyboxSky/top.jpg",
        "skyboxSky/bottom.jpg",
        "skyboxSky/front.jpg",
        "skyboxSky/back.jpg"
    };
    Texture cubemapTexture = loadCubeMap(cubetex);

    std::vector<Shader*> shaders = {
        new Shader("plane.vs", "plane.fs", "plane", planeShaderCb),
        new Shader("skybox.vs", "skybox.fs", "skybox", skyBoxShaderCb),
        new Shader("waterShader.vs", "waterShader.fs", "water", waterShaderCb),
        new Shader("flatColor.vs", "flatColor.fs", "flat", flatColorCb)
    };

    //Shader instanceShader = Shader("instanceShader.vs", "instanceShader.fs", "instance");

    View view = View(window, glm::vec3(0,20,0));
    Game *game = new Game(view);

    // TERRAIN
    std::vector<glm::vec4> colorPalette = {
        glm::vec4(201, 178, 99, 1),
        glm::vec4(135, 184, 82, 1),
        glm::vec4(80, 171, 93, 1),
        glm::vec4(120, 120, 120, 1),
        glm::vec4(200, 200, 210, 1)
    };
    ColorGenerator colorGen = ColorGenerator(colorPalette, 0.45f);
    PerlinNoise pNoise = PerlinNoise(6, 10, 0.35f);
    TerrainGenerator terrainGen = TerrainGenerator(colorGen, pNoise);

    Model *terrain = new Model(terrainGen.generateTerrain(200), "plane");
    game->addModel(terrain);

    // SKYBOX
    Mesh box = createCube(1);
    box.addTexture(cubemapTexture);
    Model *skybox = new Model(box, "skybox");
    game->addModel(skybox);

    // INSTANCES TEST
    /*
    std::vector<glm::vec3> pos = {
        glm::vec3(0,20,0),
        glm::vec3(10,20,0),
        glm::vec3(-10,20,0),
        glm::vec3(-20,20,0),
        glm::vec3(0,0,20)
    };
    Model instanceTest = Model("PlanetFirstTry.obj", instanceShader, instanceShaderCb, pos);
    game->addModel(instanceTest);
    */
    // Test
    
    //unsigned int texCont = loadTextureFromFile("container2.png");
    //Texture texContainer;
    //texContainer.id = texCont;
    //texContainer.type = "texture_diffuse";
    Mesh t = createQuad();
    //t.addTexture(texContainer);
    t.addColor(glm::vec4(0.0,0.0,1.0,0.2));
    Model *test = new Model(t, "flat");
    game->addWater(test);
    

    // WATER TEST
    unsigned int texID = loadTextureFromFile("waternormals.jpg");
    Texture waterNormal;
    waterNormal.id = texID;
    waterNormal.type = "texture_normal";
    Mesh mesh = createQuad();
    mesh.addColor(glm::vec4(0.0,0.0,1.0,0.3));
    mesh.addTexture(waterNormal);
    Model *waterTile = new Model(mesh, "water");
    glm::vec3 trans = glm::vec3(0, -5, 12);
    glm::vec3 scale = glm::vec3(8, 0, 8);
    waterTile->transform(&scale, &trans, NULL);
    game->addWater(waterTile);
    

    Scene *scene = new Scene(game, shaders);
    //scene.setAutoRotate(true);

    return scene;
}

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

    // BLENDING for transparency?
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // create starsystem scene
    //Scene *scene = createStarSystems(window);

    //create plane
    Scene *scene = createPlane(window);

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

        scene->update();
        scene->render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    } while(glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
            glfwWindowShouldClose(window) == 0);

    glfwTerminate();
    delete scene;

    return 0;
}