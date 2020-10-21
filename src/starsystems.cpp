#include <iostream>
#include <glm/gtx/string_cast.hpp>

#include "engine.hpp"
#include "utils/textureloader.hpp"

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
    std::vector<Shader *> shaders = {
        //new Shader("shader/plane.vs", "shader/plane.fs", ShaderType::SHADER_TYPE_TERRAIN, planeShaderCb),
        new Shader("shader/sun.vs", "shader/sun.fs", ShaderType::SHADER_TYPE_LIGHT, sunShaderCb),
        new Shader("shader/skybox.vs", "shader/skybox.fs", ShaderType::SHADER_TYPE_SKY, skyBoxShaderCb),
        new Shader("shader/waterShader.vs", "shader/waterShader.fs", ShaderType::SHADER_TYPE_WATER, waterShaderCb),
        new Shader("shader/waterShader.vs", "shader/waterShaderPerformance.fs", ShaderType::SHADER_TYPE_WATER_PERFORMANCE, waterShaderCb),
        new Shader("shader/instanceTerrainTest.vs", "shader/flatColor.fs", ShaderType::SHADER_TYPE_TERRAIN, flatColorCb),
        //new Shader("shader/flatColor.vs", "shader/flatColor.fs", ShaderType::SHADER_TYPE_DEFAULT, flatColorCb),
        //new Shader("shader/screenSpace.vs", "shader/postProcessAtmo.fs", ShaderType::SHADER_TYPE_POST_PROCESSOR, postProcessorAtmoCb),
        //new Shader("shader/debugNormalVector.vs", "shader/debugNormalVector.fs", nullptr, nullptr, "shader/debugNormalVector.gs", ShaderType::SHADER_TYPE_DEBUG, debugShaderCb),
        //new Shader("shader/tessVertexShader.vs", "shader/tessFragmentShader.fs", "shader/tessControlShader.tcs", "shader/tessEvalShader.tes", nullptr, ShaderType::SHADER_TYPE_DEFAULT, tessShaderCb),
        new Shader("shader/guiShader.vs", "shader/guiShader.fs", ShaderType::SHADER_TYPE_GUI, guiShaderCb)
    };
    */

    std::vector<const char *> planeShaderFiles{"shader/plane.vs", "shader/plane.fs"};
    std::vector<const char *> terrainInstanceshaderFiles{"shader/instanceTerrainTest.vs", "shader/flatColor.fs"};
    std::vector<const char *> waterShaderFilesQuality{"shader/waterShader.vs", "shader/waterShader.fs"};
    std::vector<const char *> waterShaderFilesPerf{"shader/waterShader.vs", "shader/waterShaderPerformance.fs"};
    std::vector<const char *> skyBoxShaderFiles{"shader/skybox.vs", "shader/skybox.fs"};
    std::vector<const char *> sunShaderFiles{"shader/sun.vs", "shader/sun.fs"};
    std::vector<const char *> guiShaderFiles{"shader/guiShader.vs", "shader/guiShader.fs"};

    std::vector<Shader *> shaders = {
        new TerrainInstanceShader(terrainInstanceshaderFiles, ShaderType::SHADER_TYPE_TERRAIN),
        //new TerrainPlaneShader(planeShaderFiles, ShaderType::SHADER_TYPE_TERRAIN),
        new SunShader(sunShaderFiles, ShaderType::SHADER_TYPE_LIGHT),
        new SkyBoxShader(skyBoxShaderFiles, ShaderType::SHADER_TYPE_SKY),
        new GuiShader(guiShaderFiles, ShaderType::SHADER_TYPE_GUI)
    };

    /* Camera */
    glm::vec3 camPos = glm::vec3(5, 15, 5);
    //glm::vec3 camPos = glm::vec3(5, 50, -200);
    View *view = new View(engine->getWindow(), camPos);

    /* Renderer */
    int width,height;
    view->getWindowSize(&width, &height);
    Renderer *renderer = new Renderer(shaders, width, height);
    //DEBUG
    renderer->setPolygonRenderModeWireFrame(true);

    /* Scene */
    Scene *scene = new Scene(renderer, view);

    /* Gui */
    Gui *gui = scene->getGui();
    //Texture tex = TextureLoader::loadTextureFromFile("assets/seaGround.jpg", "texture_gui");
    //gui->addGuiElement(tex, glm::vec2(0,0), glm::vec2(500,500));
    /*
    Texture tex2;
    tex2.id = renderer->DEBUG_getPostProcessingTexture();
    tex2.type = "texture_gui";
    gui->addGuiElement(tex2, glm::vec2(0,0), glm::vec2(640,360));
    */

    /* Skybox */
    Drawable *skybox = DrawableFactory::createCubeMap(cubetex, ShaderType::SHADER_TYPE_SKY);
    scene->addSky(skybox);

    /* Sun */
    Light *sun = new Light("assets/PlanetFirstTry.obj");
    glm::vec3 pos = glm::vec3(5000, 5000, 5000);
    glm::vec3 scale = glm::vec3(100,100,100);
    sun->transform(&scale, &pos, NULL);
    sun->setSpecular(glm::vec3(1.0,1.0,1.0));
    Texture sunT = TextureLoader::loadTextureFromFile("assets/sunTexture.jpg", "texture_diffuse");
    sun->addTexture(sunT);
    scene->addSun(sun);

    /* Terraingeneration */
    PerlinNoise pNoise = PerlinNoise(6, 15.0f, 0.3f, 3);
    TerrainGenerator *terrainGen = new TerrainGenerator(pNoise);


    /* Terrainmanager Test Plane */
    /*
    TerrainChunkTree *tObj = new TerrainChunkTree();
    glm::vec3 axis = glm::vec3(0,1,0);
    Drawable *d = DrawableFactory::createPrimitivePlane(axis, 16);
    d->updateInstanceSize(3);
    pos = glm::vec3(0,0,0);
    d->transform(0, NULL, &pos, NULL);
    pos = glm::vec3(16,0,0);
    d->transform(1, NULL, &pos, NULL);
    pos = glm::vec3(0,0,16);
    d->transform(2, NULL, &pos, NULL);
    d->updateMeshInstances();
    tObj->addTerrainChunk(d);
    scene->getTerrainManager()->addTerrainObject((TerrainObject*) tObj);
    */

    /* Terrainmanager EndlessPlane */
    EndlessPlane *p = new EndlessPlane(terrainGen);
    scene->getTerrainManager()->addTerrainObject((TerrainObject *) p);

    /* Terrainmanager Planet */
    /*
    terrainGen->setSphereOrigin(glm::vec3(0,0,0));
    terrainGen->setSphereRadius(64);
    Planet *planet = new Planet(terrainGen);
    scene->getTerrainManager()->addTerrainObject((TerrainObject*) planet);
    */

    // Water Tessellation test
    /*
    Drawable *planeWater = new TerrainTile(terrainGen, 60, glm::vec3(0,0,0), 12, GenerationType::PLANE_FLAT, ShaderType::SHADER_TYPE_DEFAULT);
    planeWater->setMeshDrawMode(MeshDrawMode::DRAW_MODE_TESSELLATION);
    game->addEntity(planeWater);
    Drawable *waterTile = DrawableFactory::createWaterTile(glm::vec3(0,0,0), 120, glm::vec3(0,0,1));
    TerrainTile *waterTile = new TerrainTile(terrainGen, 60, glm::vec3(0,0,0), 12, GenerationType::PLANE_FLAT, ShaderType::SHADER_TYPE_WATER);
    game->addWater(waterTile);
    */

    return scene;
}

int main() {
    Engine engine = Engine(1920, 1080, "starsystem");
    if (!engine.getWindow()) {
        fprintf(stdout, "Unable to create window\n");
        return 0;
    }

    Scene *scene = createPlane(&engine);

    if (!scene) {
        fprintf(stdout, "Scene creation failed\n");
        return 0;
    }

    engine.addScene(scene);
    fprintf(stdout, "Starting render loop...\n");

    engine.render();

    return 0;
}