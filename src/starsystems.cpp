#include <iostream>
#include <glm/gtx/string_cast.hpp>

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
    Drawable *sun = new Light("assets/PlanetFirstTry.obj");
    glm::vec3 pos1 = glm::vec3(0, 1000, 1000);
    glm::vec3 scale1 = glm::vec3(100,100,100);
    sun->transform(&scale1, &pos1, NULL);

    // Skybox
    Drawable *skybox = DrawableFactory::createCubeMap(cubetex, ShaderType::SHADER_TYPE_SKY);

    // Add planets
    // These could be instances
    std::vector<Mesh*> planetMeshes;
    if (!AssetLoader::loadModel("assets/PlanetFirstTry.obj", &planetMeshes)) {
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
    shader->uniform("light.ambient", glm::vec3(0.1, 0.1, 0.1));
    shader->uniform("light.diffuse", glm::vec3(0.8, 0.8, 0.8));
    shader->uniform("light.specular", glm::vec3(1.0, 1.0, 1.0));
}

void flatColorCb(Shader *shader, Drawable *drawable, Game *game) {
    shader->uniform("sunPos", game->getSun()->getPosition());
    shader->uniform("MVP", game->getView().getProjectionMatrix() * game->getView().getCameraMatrix() *
                               drawable->getModelMatrix());
    shader->uniform("VP", game->getView().getProjectionMatrix() * game->getView().getCameraMatrix());
    shader->uniform("modelMatrix", drawable->getModelMatrix());
    shader->uniform("cameraMatrix", game->getView().getCameraMatrix());
    shader->uniform("projectionMatrix", game->getView().getProjectionMatrix());
    shader->uniform("lowerBound", -10.0f);
    shader->uniform("upperBound", 20.0f);
    shader->uniform("gridDimension", 64.0f);
    shader->uniform("gridOrigin", glm::vec3(0,0,0));
}

void instanceShaderCb(Shader *shader, Drawable *drawable, Game *game) {
    shader->uniform("projection", game->getView().getProjectionMatrix());
    shader->uniform("camera", game->getView().getCameraMatrix());
}

void waterShaderCb(Shader *shader, Drawable *drawable, Game *game) {
    Light *light = dynamic_cast<Light *>(game->getSun());
    shader->uniform("MVP", game->getView().getProjectionMatrix() * game->getView().getCameraMatrix() *
                               drawable->getModelMatrix());
    shader->uniform("cameraMatrix", game->getView().getCameraMatrix());
    shader->uniform("projectionMatrix", game->getView().getProjectionMatrix());
    shader->uniform("worldNormal", game->getView().getWorldNormal());
    shader->uniform("normalMatrix", drawable->getNormalMatrix());
    shader->uniform("modelMatrix", drawable->getModelMatrix());
    shader->uniform("cameraPos", game->getView().getCameraPosition());
    shader->uniform("light.position", light->getPosition());
    shader->uniform("light.color", light->getColor());
    shader->uniform("light.ambient", light->getAmbient());
    shader->uniform("light.diffuse", light->getDiffuse());
    shader->uniform("light.specular", light->getSpecular());
    shader->uniform("tilingSize", drawable->getScale().x / 4.0f);
    shader->uniform("nearPlane", game->getView().getNearPlane());
    shader->uniform("farPlane", game->getView().getFarPlane());
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
    //shader->uniform("sunPosition", game->getSun()->getPosition());
    shader->uniform("sunPosition", glm::vec3(10000000.0f, 10000000.0f, 10000000.0f));
}

void debugShaderCb(Shader *shader, Drawable *drawable, Game *game) {
    shader->uniform("modelMatrix", drawable->getModelMatrix());
    shader->uniform("cameraMatrix", game->getView().getCameraMatrix());
    shader->uniform("projectionMatrix", game->getView().getProjectionMatrix());
}

void tessShaderCb(Shader *shader, Drawable *drawable, Game *game) {
    Light *light = dynamic_cast<Light *>(game->getSun());
    shader->uniform("modelMatrix", drawable->getModelMatrix());
    shader->uniform("cameraMatrix", game->getView().getCameraMatrix());
    shader->uniform("cameraPos", game->getView().getCameraPosition());
    shader->uniform("projectionMatrix", game->getView().getProjectionMatrix());
    shader->uniform("light.position", light->getPosition());
    shader->uniform("light.color", light->getColor());
    shader->uniform("light.ambient", light->getAmbient());
    shader->uniform("light.diffuse", light->getDiffuse());
    shader->uniform("light.specular", light->getSpecular());
    shader->uniform("frameTime", g_currentFrameTime);
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
        new Shader("shader/sun.vs", "shader/sun.fs", ShaderType::SHADER_TYPE_LIGHT, sunShaderCb),
        new Shader("shader/skybox.vs", "shader/skybox.fs", ShaderType::SHADER_TYPE_SKY, skyBoxShaderCb),
        new Shader("shader/waterShader.vs", "shader/waterShader.fs", ShaderType::SHADER_TYPE_WATER, waterShaderCb),
        new Shader("shader/waterShader.vs", "shader/waterShaderPerformance.fs", ShaderType::SHADER_TYPE_WATER_PERFORMANCE, waterShaderCb),
        new Shader("shader/testVertexTexture.vs", "shader/flatColor.fs", ShaderType::SHADER_TYPE_DEFAULT, flatColorCb),
        //new Shader("shader/screenSpace.vs", "shader/postProcessAtmo.fs", ShaderType::SHADER_TYPE_POST_PROCESSOR, postProcessorAtmoCb),
        //new Shader("shader/debugNormalVector.vs", "shader/debugNormalVector.fs", nullptr, nullptr, "shader/debugNormalVector.gs", ShaderType::SHADER_TYPE_DEBUG, debugShaderCb),
        //new Shader("shader/tessVertexShader.vs", "shader/tessFragmentShader.fs", "shader/tessControlShader.tcs", "shader/tessEvalShader.tes", nullptr, ShaderType::SHADER_TYPE_DEFAULT, tessShaderCb),
        new Shader("shader/guiShader.vs", "shader/guiShader.fs", ShaderType::SHADER_TYPE_GUI, guiShaderCb)
    };

    //Light *sun = new Light(glm::vec3(200000, 200000, 10000));
    Light *sun = new Light("assets/PlanetFirstTry.obj");
    glm::vec3 pos = glm::vec3(5000, 5000, 5000);
    glm::vec3 scale = glm::vec3(100,100,100);
    sun->transform(&scale, &pos, NULL);
    sun->setSpecular(glm::vec3(1.0,1.0,1.0));

    Texture sunT = TextureLoader::loadTextureFromFile("assets/sunTexture.jpg", "texture_diffuse");
    sun->addTexture(sunT);
    

    glm::vec3 camPos = glm::vec3(0, 20, -20);
    //glm::vec3 camPos = glm::vec3(0, 700, -1300);
    View view = View(engine->getWindow(), camPos);
    Game *game = new Game(view);
    game->addSun(sun);

    //PerlinNoise pNoise = PerlinNoise(6, 10.0f, 0.01f, 0);
    PerlinNoise pNoise = PerlinNoise(6, 15.0f, 0.3f, 3);
    TerrainGenerator *terrainGen = new TerrainGenerator(pNoise);
    //TerrainManager *terr = new TerrainManager(terrainGen, 960, 0, TerrainType::SPHERE, glm::vec3(0,0,0));
    //game->addTerrainManager(terr);
    

    // Random Entity Test
    /*
    Drawable *cube = DrawableFactory::createPrimitive(PrimitiveType::CUBE, ShaderType::SHADER_TYPE_DEFAULT);
    glm::vec3 posC = glm::vec3(-3, 0, -3);
    glm::vec3 scaleC = glm::vec3(0.5,0.5,0.5);
    cube->transform(&scaleC, &posC, NULL);
    game->addEntity(cube);
    */
    //cube->addTexture(TextureLoader::loadTextureFromFile("assets/waternormals.jpg", "texture_normal"));

    // Sphere Segment
    /*
    terrainGen->setSphereRadius(60);
    terrainGen->setSphereOrigin(glm::vec3(0,0,0));
    TerrainTile *t = new TerrainTile(terrainGen, 120, glm::vec3(0,0,0), 1, glm::vec3(0,1,0), GenerationType::SPHERE, ShaderType::SHADER_TYPE_TERRAIN);
    TerrainTile *t2 = new TerrainTile(terrainGen, 120, glm::vec3(0,0,0), 1, glm::vec3(0,-1,0), GenerationType::SPHERE, ShaderType::SHADER_TYPE_TERRAIN);
    TerrainTile *t3 = new TerrainTile(terrainGen, 120, glm::vec3(0,0,0), 1, glm::vec3(1,0,0), GenerationType::SPHERE, ShaderType::SHADER_TYPE_TERRAIN);
    TerrainTile *t4 = new TerrainTile(terrainGen, 120, glm::vec3(0,0,0), 1, glm::vec3(-1,0,0), GenerationType::SPHERE, ShaderType::SHADER_TYPE_TERRAIN);
    TerrainTile *t5 = new TerrainTile(terrainGen, 120, glm::vec3(0,0,0), 1, glm::vec3(0,0,1), GenerationType::SPHERE, ShaderType::SHADER_TYPE_TERRAIN);
    TerrainTile *t6 = new TerrainTile(terrainGen, 120, glm::vec3(0,0,0), 1, glm::vec3(0,0,-1), GenerationType::SPHERE, ShaderType::SHADER_TYPE_TERRAIN);
    game->addTerrain(t);
    game->addTerrain(t2);
    game->addTerrain(t3);
    game->addTerrain(t4);
    game->addTerrain(t5);
    game->addTerrain(t6);
    */
    
    TerrainTile *terr1 = new TerrainTile(terrainGen, 32, glm::vec3(0,0,32), 1, GenerationType::PLANE, ShaderType::SHADER_TYPE_TERRAIN);

    //Drawable *t = DrawableFactory::createPrimitive(PrimitiveType::PLANE, ShaderType::SHADER_TYPE_DEFAULT, 32);
    Drawable *t = new TerrainTile(terrainGen, 32, glm::vec3(0,0,0), 1, GenerationType::PLANE, ShaderType::SHADER_TYPE_DEFAULT);
    //Drawable *t1 = DrawableFactory::createPrimitive(PrimitiveType::PLANE, ShaderType::SHADER_TYPE_DEFAULT, 32);
    Drawable *t1 = new TerrainTile(terrainGen, 32, glm::vec3(0,0,0), 1, GenerationType::PLANE, ShaderType::SHADER_TYPE_DEFAULT);
    glm::vec3 transP = glm::vec3(31, 0, 0);
    t1->transform(NULL, &transP, NULL);
    //transP = glm::vec3(0,0,32);
    //terr1->transform(NULL, &transP, NULL);
    
    HeightMap *m = new HeightMap(terrainGen, glm::vec3(0,0,0), glm::vec3(0,1,0), 64, 1);
    
    Texture hmap;
    Texture nMap;
    hmap.id = m->getHeightTexture();
    hmap.type = "texture_height";
    nMap.id = m->getNormalTexture();
    nMap.type = "texture_normal";
    t->addTexture(hmap);
    t->addTexture(nMap);
    t1->addTexture(hmap);
    t1->addTexture(nMap);
    
    game->addEntity(t);
    game->addEntity(t1);
    
    game->addTerrain(terr1);
    
    // Plane
    /*
    TerrainTile *t = new TerrainTile(terrainGen, 60, glm::vec3(0,0,0), 1, GenerationType::PLANE, ShaderType::SHADER_TYPE_TERRAIN);
    TerrainTile *t2 = new TerrainTile(terrainGen, 60, glm::vec3(0,0,60), 2, GenerationType::PLANE, ShaderType::SHADER_TYPE_TERRAIN);
    TerrainTile *t3 = new TerrainTile(terrainGen, 60, glm::vec3(0,0,120), 4, GenerationType::PLANE, ShaderType::SHADER_TYPE_TERRAIN);
    TerrainTile *t4 = new TerrainTile(terrainGen, 60, glm::vec3(0,0,180), 6, GenerationType::PLANE, ShaderType::SHADER_TYPE_TERRAIN);
    game->addTerrain(t);
    game->addTerrain(t2);
    game->addTerrain(t3);
    game->addTerrain(t4);
    */
    //game->addTerrain(t4);
    //Drawable *planeWater = new TerrainTile(terrainGen, 60, glm::vec3(0,0,0), 12, GenerationType::PLANE_FLAT, ShaderType::SHADER_TYPE_DEFAULT);
    //planeWater->setMeshDrawMode(MeshDrawMode::DRAW_MODE_TESSELLATION);
    //game->addEntity(planeWater);
    
    // SKYBOX
    Drawable *skybox = DrawableFactory::createCubeMap(cubetex, ShaderType::SHADER_TYPE_SKY);
    game->addSky(skybox);

    // GUI
    Gui *gui = new Gui();
    Texture tex = TextureLoader::loadTextureFromFile("assets/seaGround.jpg", "texture_gui");
    //gui->addGuiElement(tex, glm::vec2(0,0), glm::vec2(500,500));

    // WATER TEST
    //Drawable *waterTile = DrawableFactory::createWaterTile(glm::vec3(0,0,0), 120, glm::vec3(0,0,1));
    //TerrainTile *waterTile = new TerrainTile(terrainGen, 60, glm::vec3(0,0,0), 12, GenerationType::PLANE_FLAT, ShaderType::SHADER_TYPE_WATER);
    //game->addWater(waterTile);

    int width,height;
    view.getWindowSize(&width, &height);
    Renderer *renderer = new Renderer(shaders, width, height);
    //DEBUG
    renderer->setPolygonRenderModeWireFrame(true);
    Texture tex2;
    tex2.id = renderer->DEBUG_getPostProcessingTexture();
    tex2.type = "texture_gui";
    //gui->addGuiElement(tex2, glm::vec2(0,0), glm::vec2(640,360));
    // End Debug
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
    Engine engine = Engine(1920, 1080, "starsystem");
    if (!engine.getWindow())
        return 0;

    // create starsystem scene
    //Scene *scene = createStarSystems(&engine);

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