#include <iostream>

#include "engine.hpp"
#include "planet.hpp"

/*
 *
 */
void standardShadingCb(Shader *shader, Drawable *drawable, Game *game) {
    shader->uniform("MVP", game->getView().getProjectionMatrix() * game->getView().getCameraMatrix() *
                               drawable->getModelMatrix());

    shader->uniform("modelMatrix", drawable->getModelMatrix());
    shader->uniform("normalMatrix", drawable->getNormalMatrix());
    shader->uniform("cameraPos", game->getView().getCameraPosition());

    // shader_.uniform("objectColor", glm::vec3(1.0f, 0.5f, 0.31f));
    shader->uniform("material.ambient", glm::vec3(1.0f, 0.5f, 0.31f));
    shader->uniform("material.diffuse", glm::vec3(1.0f, 0.5f, 0.31f));
    shader->uniform("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
    shader->uniform("material.shininess", 32.0f);
    shader->uniform("light.position", game->getLightSource()->getPosition());
    shader->uniform("light.ambient", glm::vec3(0.1f, 0.1f, 0.1f));
    shader->uniform("light.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
    shader->uniform("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));
}

void sunShaderCb(Shader *shader, Drawable *drawable, Game *game) {
    Light *light = dynamic_cast<Light *>(drawable);
    shader->uniform("MVP", game->getView().getProjectionMatrix() * game->getView().getCameraMatrix() *
                               drawable->getModelMatrix());
    shader->uniform("color", light->getColor());
}

void skyBoxShaderCb(Shader *shader, Drawable *drawable, Game *game) {
    glm::mat4 mvp = game->getView().getProjectionMatrix() * glm::mat4(glm::mat3(game->getView().getCameraMatrix())) *
                    drawable->getModelMatrix();

    shader->uniform("modelMatrix", drawable->getModelMatrix());
    shader->uniform("MVP", mvp);
}

Scene *createStarSystems(Engine *engine) {

    std::vector<std::string> cubetex = {"assets/skyboxSpace/right.png", "assets/skyboxSpace/left.png",
                                        "assets/skyboxSpace/top.png",   "assets/skyboxSpace/bottom.png",
                                        "assets/skyboxSpace/front.png", "assets/skyboxSpace/back.png"};

    // Create and compile shaders
    std::vector<Shader *> shaders = {
        new Shader("shader/StandardShading.vs", "shader/StandardShading.fs", "planet", standardShadingCb),
        new Shader("shader/sun.vs", "shader/sun.fs", SHADER_TYPE_LIGHT, sunShaderCb),
        new Shader("shader/skybox.vs", "shader/skybox.fs", SHADER_TYPE_SKY, skyBoxShaderCb)};

    View view = View(engine->getWindow(), glm::vec3(0.0f, 20.0f, -40.0f));

    // Sun
    Drawable *sun = DrawableFactory::createModel("PlanetFirstTry.obj", SHADER_TYPE_LIGHT);

    // Skybox
    Drawable *skybox = DrawableFactory::createCubeMap(cubetex, SHADER_TYPE_SKY);

    // Add planets
    // These could be instances
    std::vector<Mesh> planetMeshes;
    if (!AssetLoader::loadModel("PlanetFirstTry.obj", &planetMeshes)) {
        std::cout << "Could not load model" << std::endl;
        return NULL;
    }

    Planet *planet1 = new Planet(planetMeshes, 0.7f, glm::vec3(10, 0, 0));
    Planet *planet2 = new Planet(planetMeshes, 0.5f, glm::vec3(20, 0, 20));
    Planet *planet3 = new Planet(planetMeshes, 0.2f, glm::vec3(30, 0, 40));

    glm::vec3 scale = glm::vec3(0.5f, 0.5f, 0.5f);
    planet1->transform(&scale, NULL, NULL);
    planet2->transform(&scale, NULL, NULL);
    planet3->transform(&scale, NULL, NULL);
    // INSTANCES TEST, load asteroids
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

    Game *game = new Game(view);
    game->addLight(sun);
    game->addSky(skybox);
    game->addEntity(planet1);
    game->addEntity(planet2);
    game->addEntity(planet3);

    Renderer *renderer = new Renderer(shaders);
    Scene *scene = new Scene(game, renderer);

    return scene;
}

void planeShaderCb(Shader *shader, Drawable *drawable, Game *game) {
    shader->uniform("MVP", game->getView().getProjectionMatrix() * game->getView().getCameraMatrix() *
                               drawable->getModelMatrix());
    shader->uniform("normalMatrix", drawable->getNormalMatrix());
    shader->uniform("modelMatrix", drawable->getModelMatrix());
    shader->uniform("cameraPos", game->getView().getCameraPosition());
    shader->uniform("light.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
    shader->uniform("light.ambient", glm::vec3(1.0, 1.0, 1.0));
    shader->uniform("light.diffuse", glm::vec3(1.0, 1.0, 1.0));
    shader->uniform("light.specular", glm::vec3(1.0, 1.0, 1.0));
}

void flatColorCb(Shader *shader, Drawable *drawable, Game *game) {
    shader->uniform("MVP", game->getView().getProjectionMatrix() * game->getView().getCameraMatrix() *
                               drawable->getModelMatrix());
}

void instanceShaderCb(Shader *shader, Drawable *drawable, Game *game) {
    shader->uniform("projection", game->getView().getProjectionMatrix());
    shader->uniform("camera", game->getView().getCameraMatrix());
}

void waterShaderCb(Shader *shader, Drawable *drawable, Game *game) {
    shader->uniform("MVP", game->getView().getProjectionMatrix() * game->getView().getCameraMatrix() *
                               drawable->getModelMatrix());
    shader->uniform("worldNormal", game->getView().getWorldNormal());
    shader->uniform("normalMatrix", drawable->getNormalMatrix());
    shader->uniform("modelMatrix", drawable->getModelMatrix());
    shader->uniform("cameraPos", game->getView().getCameraPosition());
    shader->uniform("light.position", game->getSun()->getPosition());
    shader->uniform("light.color", game->getSun()->getPosition());
    shader->uniform("tilingSize", drawable->getScale().x / 4.0f);
}

void guiShaderCb(Shader *shader, Drawable *drawable, Game *game) {
    shader->uniform("MVP", game->getView().getOrthoProjection() * drawable->getModelMatrix());
}

Scene *createPlane(Engine *engine) {
    std::cout << "Create Plane." << std::endl;
    /*
    std::vector<std::string> cubetex = {
        "assets/skyboxSky/right.jpg",
        "assets/skyboxSky/left.jpg",
        "assets/skyboxSky/top.jpg",
        "assets/skyboxSky/bottom.jpg",
        "assets/skyboxSky/front.jpg",
        "assets/skyboxSky/back.jpg"
    };
    */
    std::vector<std::string> cubetex = {"assets/skyboxSky2/right.png", "assets/skyboxSky2/left.png",
                                        "assets/skyboxSky2/top.png",   "assets/skyboxSky2/bottom.png",
                                        "assets/skyboxSky2/front.png", "assets/skyboxSky2/back.png"};

    std::vector<Shader *> shaders = {
        new Shader("shader/plane.vs", "shader/plane.fs", SHADER_TYPE_TERRAIN, planeShaderCb),
        new Shader("shader/skybox.vs", "shader/skybox.fs", SHADER_TYPE_SKY, skyBoxShaderCb),
        new Shader("shader/waterShader.vs", "shader/waterShader.fs", SHADER_TYPE_WATER, waterShaderCb),
        // new Shader("waterShader.vs", "waterShaderPerformance.fs", SHADER_TYPE_WATER_PERFORMANCE, waterShaderCb),
        new Shader("shader/flatColor.vs", "shader/flatColor.fs", "flat", flatColorCb),
        new Shader("shader/guiShader.vs", "shader/guiShader.fs", SHADER_TYPE_GUI, guiShaderCb)};

    Light *sun = new Light(glm::vec3(200000, 200000, 10000));

    View view = View(engine->getWindow(), glm::vec3(0, 20, -20));
    Game *game = new Game(view);
    game->addSun(sun);

    // Terrain
    Terrain *terrain = new Terrain(500);
    game->addTerrain(terrain);

    // SKYBOX
    Drawable *skybox = DrawableFactory::createCubeMap(cubetex, SHADER_TYPE_SKY);
    game->addSky(skybox);

    // GUI
    Gui *gui = new Gui();

    // WATER TEST
    Drawable *waterTile = DrawableFactory::createPrimitive(PrimitiveType::QUAD, SHADER_TYPE_WATER);
    waterTile->addColor(glm::vec4(0.0, 0.0, 1.0, 0.6));
    glm::vec3 trans = glm::vec3(0, 0, 0);
    glm::vec3 scale = glm::vec3(300, 1, 300);
    waterTile->transform(&scale, &trans, NULL);
    game->addWater(waterTile);

    Renderer *renderer = new Renderer(shaders);
    Scene *scene = new Scene(game, renderer);
    scene->addGui(gui);

    return scene;
}

Scene *test(Engine *engine) {
    std::vector<std::string> cubetex = {"assets/skyboxSky2/right.png", "assets/skyboxSky2/left.png",
                                        "assets/skyboxSky2/top.png",   "assets/skyboxSky2/bottom.png",
                                        "assets/skyboxSky2/front.png", "assets/skyboxSky2/back.png"};
    std::vector<Shader *> shaders = {
        new Shader("shader/skybox.vs", "shader/skybox.fs", SHADER_TYPE_SKY, skyBoxShaderCb),
        new Shader("shader/sun.vs", "shader/sun.fs", SHADER_TYPE_LIGHT, sunShaderCb),
    };

    View view = View(engine->getWindow(), glm::vec3(0, 20, -20));
    Game *game = new Game(view);

    Drawable *light = DrawableFactory::createLight("assets/PlanetFirstTry.obj", SHADER_TYPE_LIGHT);
    Drawable *skybox = DrawableFactory::createCubeMap(cubetex, SHADER_TYPE_SKY);
    game->addSky(skybox);
    game->addSun(light);

    Renderer *renderer = new Renderer(shaders);
    Scene *scene = new Scene(game, renderer);

    return scene;
}

int main() {
    Engine engine = Engine(1280, 720, "starsystem");
    if (!engine.getWindow())
        return 0;

    // create starsystem scene
    // Scene *scene = createStarSystems(engine.getWindow);

    // create plane
    Scene *scene = createPlane(&engine);

    // Scene *scene = test(&engine);

    if (!scene)
        return 0;

    std::cout << "Starting render loop..." << std::endl;

    engine.addScene(scene);

    engine.render();

    return 0;
}