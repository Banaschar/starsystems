#include <iostream>

#include "engine.hpp"
#include "utils/textureloader.hpp"
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
        new Shader("shader/StandardShading.vs", "shader/StandardShading.fs", ShaderType::SHADER_TYPE_DEFAULT, standardShadingCb),
        new Shader("shader/sun.vs", "shader/sun.fs", ShaderType::SHADER_TYPE_LIGHT, sunShaderCb),
        new Shader("shader/skybox.vs", "shader/skybox.fs", ShaderType::SHADER_TYPE_SKY, skyBoxShaderCb)};

    View view = View(engine->getWindow(), glm::vec3(0.0f, 20.0f, -40.0f));

    // Sun
    Drawable *sun = DrawableFactory::createModel("PlanetFirstTry.obj", ShaderType::SHADER_TYPE_LIGHT);

    // Skybox
    Drawable *skybox = DrawableFactory::createCubeMap(cubetex, ShaderType::SHADER_TYPE_SKY);

    // Add planets
    // These could be instances
    std::vector<Mesh*> planetMeshes;
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

    int width,height;
    view.getWindowSize(&width, &height);
    Renderer *renderer = new Renderer(shaders, width, height);
    Scene *scene = new Scene(game, renderer);

    return scene;
}

void planeShaderCb(Shader *shader, Drawable *drawable, Game *game) {
    shader->uniform("MVP", game->getView().getProjectionMatrix() * game->getView().getCameraMatrix() *
                               drawable->getModelMatrix());
    shader->uniform("normalMatrix", drawable->getNormalMatrix());
    shader->uniform("modelMatrix", drawable->getModelMatrix());
    shader->uniform("cameraPos", game->getView().getCameraPosition());
    shader->uniform("light.position", game->getSun()->getPosition());
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

void postProcessorAtmoCb(Shader *shader, Drawable *drawable, Game *game) {
    shader->uniform("MVP", game->getView().getProjectionMatrix() * game->getView().getCameraMatrix() *
                               drawable->getModelMatrix());
    shader->uniform("cameraMatrix", game->getView().getCameraMatrix());
    shader->uniform("projectionMatrix", game->getView().getProjectionMatrix());
    shader->uniform("modelMatrix", drawable->getModelMatrix());
    shader->uniform("positionOriginal", drawable->getPosition());
}

void debugShaderCb(Shader *shader, Drawable *drawable, Game *game) {
    shader->uniform("modelMatrix", drawable->getModelMatrix());
    shader->uniform("cameraMatrix", game->getView().getCameraMatrix());
    shader->uniform("projectionMatrix", game->getView().getProjectionMatrix());
}

Scene *createPlane(Engine *engine) {
    std::cout << "Create Plane." << std::endl;
    
    std::vector<std::string> cubetex = {
        "assets/skyboxSpace/right.png",
        "assets/skyboxSpace/left.png",
        "assets/skyboxSpace/top.png",
        "assets/skyboxSpace/bottom.png",
        "assets/skyboxSpace/front.png",
        "assets/skyboxSpace/back.png"
    };
    /*
    std::vector<std::string> cubetex = {"assets/skyboxSky2/right.png", "assets/skyboxSky2/left.png",
                                        "assets/skyboxSky2/top.png",   "assets/skyboxSky2/bottom.png",
                                        "assets/skyboxSky2/front.png", "assets/skyboxSky2/back.png"};
    */
    std::vector<Shader *> shaders = {
        new Shader("shader/plane.vs", "shader/plane.fs", ShaderType::SHADER_TYPE_TERRAIN, planeShaderCb),
        new Shader("shader/skybox.vs", "shader/skybox.fs", ShaderType::SHADER_TYPE_SKY, skyBoxShaderCb),
        new Shader("shader/waterShader.vs", "shader/waterShader.fs", ShaderType::SHADER_TYPE_WATER, waterShaderCb),
        new Shader("shader/waterShader.vs", "shader/waterShaderPerformance.fs", ShaderType::SHADER_TYPE_WATER_PERFORMANCE, waterShaderCb),
        //new Shader("shader/flatColor.vs", "shader/flatColor.fs", ShaderType::SHADER_TYPE_DEFAULT, flatColorCb),
        new Shader("shader/guiShader.vs", "shader/guiShader.fs", ShaderType::SHADER_TYPE_GUI, guiShaderCb),
        //new Shader("shader/screenSpace.vs", "shader/postProcessAtmo.fs", ShaderType::SHADER_TYPE_POST_PROCESSOR, postProcessorAtmoCb),
        new Shader("shader/debugNormalVector.vs", "shader/debugNormalVector.fs", ShaderType::SHADER_TYPE_DEBUG, debugShaderCb, "shader/debugNormalVector.gs")
    };

    Light *sun = new Light(glm::vec3(200000, 200000, 10000));

    //glm::vec3 camPos = glm::vec3(0, 20, -20);
    glm::vec3 camPos = glm::vec3(0, 700, -1300);
    View view = View(engine->getWindow(), camPos);
    Game *game = new Game(view);
    game->addSun(sun);

    //PerlinNoise pNoise = PerlinNoise(6, 10.0f, 0.01f, 0);
    PerlinNoise pNoise = PerlinNoise(6, 20.0f, 0.45f, 3);
    TerrainGenerator *terrainGen = new TerrainGenerator(pNoise);
    TerrainManager *terr = new TerrainManager(terrainGen, 960, 0, TerrainType::SPHERE, glm::vec3(0,0,0));
    game->addTerrainManager(terr);
    
    //TerrainTile *t = new TerrainTile(terrainGen, 241, glm::vec3(0,0,0), 1, GenerationType::PLANE, ShaderType::SHADER_TYPE_TERRAIN);
    //game->addTerrain(t);

    // Random Entity Test
    //Drawable *cube = DrawableFactory::createPrimitive(PrimitiveType::CUBE, ShaderType::SHADER_TYPE_DEFAULT);
    //glm::vec3 scale = glm::vec3(2,2,2);
    //cube->transform(&scale, NULL, NULL);
    //game->addEntity(cube);
    
    // SKYBOX
    Drawable *skybox = DrawableFactory::createCubeMap(cubetex, ShaderType::SHADER_TYPE_SKY);
    game->addSky(skybox);

    // GUI
    Gui *gui = new Gui();
    //tex = TextureLoader::loadTextureFromFile("assets/seaGround.jpg", "texture_gui");
    //gui->addGuiElement(tex, glm::vec2(100,1), glm::vec2(200,200));

    // WATER TEST
    //Drawable *waterTile = DrawableFactory::createWaterTile(glm::vec3(0,0,0), 120, glm::vec3(0,0,1));
    //TerrainTile *waterTile = new TerrainTile(terrainGen, 241, glm::vec3(0,0,0), 12, GenerationType::PLANE_FLAT, ShaderType::SHADER_TYPE_WATER);
    //game->addWater(waterTile);

    int width,height;
    view.getWindowSize(&width, &height);
    Renderer *renderer = new Renderer(shaders, width, height);
    Scene *scene = new Scene(game, renderer);
    scene->addGui(gui);

    return scene;
}

Scene *test(Engine *engine) {
    std::vector<std::string> cubetex = {"assets/skyboxSky2/right.png", "assets/skyboxSky2/left.png",
                                        "assets/skyboxSky2/top.png",   "assets/skyboxSky2/bottom.png",
                                        "assets/skyboxSky2/front.png", "assets/skyboxSky2/back.png"};
    std::vector<Shader *> shaders = {
        new Shader("shader/skybox.vs", "shader/skybox.fs", ShaderType::SHADER_TYPE_SKY, skyBoxShaderCb),
        new Shader("shader/sun.vs", "shader/sun.fs", ShaderType::SHADER_TYPE_LIGHT, sunShaderCb),
    };

    View view = View(engine->getWindow(), glm::vec3(0, 20, -20));
    Game *game = new Game(view);

    Drawable *light = DrawableFactory::createLight("assets/PlanetFirstTry.obj", ShaderType::SHADER_TYPE_LIGHT);
    Drawable *skybox = DrawableFactory::createCubeMap(cubetex, ShaderType::SHADER_TYPE_SKY);
    game->addSky(skybox);
    game->addSun(light);

    int width,height;
    view.getWindowSize(&width, &height);
    Renderer *renderer = new Renderer(shaders, width, height);
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