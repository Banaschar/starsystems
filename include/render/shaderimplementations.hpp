#ifndef SHADERIMPLEMENTATIONS_HPP
#define SHADERIMPLEMENTATIONS_HPP

#include "shader.hpp"
#include "scenerenderdata.hpp"
#include "drawable.hpp"
#include "light.hpp"
#include "global.hpp"

class TerrainInstanceShader : public Shader {
public:
    TerrainInstanceShader(std::vector<const char *> &shaderFiles, ShaderType type) : Shader(shaderFiles, type) {}
    void setSceneUniforms(SceneRenderData &sceneData, void *data) override {
        uniform("sunPos", sceneData.sun->getPosition());
        uniform("cameraMatrix", sceneData.view->getCameraMatrix());
        uniform("VP", sceneData.view->getProjectionMatrix() * sceneData.view->getCameraMatrix());
        uniform("cameraPos", sceneData.view->getCameraPosition());
        uniform("lowerBound", -10.0f);
        uniform("upperBound", 20.0f);
        uniform("heightMapDimension", 256.0f);
        uniform("meshDimension", 16.0f);
        uniform("gridOrigin", glm::vec3(0,0,0));
    }
    void setDrawableUniforms(SceneRenderData &sceneData, Drawable *drawable, void *data) override {
        ;
    }
};

class TerrainPlaneShader : public Shader {
public:
    TerrainPlaneShader(std::vector<const char *> &shaderFiles, ShaderType type) : Shader(shaderFiles, type) {}
    void setSceneUniforms(SceneRenderData &sceneData, void *data) {
        TerrainObjectAttributes *attribs = reinterpret_cast<TerrainObjectAttributes *>(data);
        if (attribs) {
            //uniform("amplitude", attribs->generationData->amplitude);
            //uniform("tiling", (float)terrain->getDimension() / 2.0f);
            //uniform("waterLevel", attribs->generationData->waterLevel);
            uniform("sphereRadius", attribs->bodyRadius);
            uniform("sphereOrigin", attribs->bodyOrigin);
        }

        if (sceneData.clipPlane)
            uniform("clipPlane", *(sceneData.clipPlane));

        Light *light = dynamic_cast<Light *>(sceneData.sun);
        uniform("cameraPos", sceneData.view->getCameraPosition());
        uniform("light.position", light->getPosition());
        uniform("light.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
        uniform("light.ambient", glm::vec3(0.1, 0.1, 0.1));
        uniform("light.diffuse", glm::vec3(0.8, 0.8, 0.8));
        uniform("light.specular", glm::vec3(1.0, 1.0, 1.0));
    }
    void setDrawableUniforms(SceneRenderData &sceneData, Drawable *drawable, void *data) {
        uniform("MVP", sceneData.view->getProjectionMatrix() * sceneData.view->getCameraMatrix() *
                               drawable->getModelMatrix());
        uniform("normalMatrix", drawable->getNormalMatrix());
        uniform("modelMatrix", drawable->getModelMatrix());
    }
};

class WaterShader : public Shader {
public:
    WaterShader(std::vector<const char *> &shaderFiles, ShaderType type) : Shader(shaderFiles, type) {}
    void setSceneUniforms(SceneRenderData &sceneData, void *data) {
        Light *light = dynamic_cast<Light *>(sceneData.sun);

        uniform("cameraMatrix", sceneData.view->getCameraMatrix());
        uniform("projectionMatrix", sceneData.view->getProjectionMatrix());
        uniform("worldNormal", sceneData.view->getWorldNormal());
        uniform("cameraPos", sceneData.view->getCameraPosition());
        uniform("light.position", light->getPosition());
        uniform("light.color", light->getColor());
        uniform("light.ambient", light->getAmbient());
        uniform("light.diffuse", light->getDiffuse());
        uniform("light.specular", light->getSpecular());
        uniform("nearPlane", sceneData.view->getNearPlane());
        uniform("farPlane", sceneData.view->getFarPlane());
    }
    void setDrawableUniforms(SceneRenderData &sceneData, Drawable *drawable, void *data) {
        uniform("MVP", sceneData.view->getProjectionMatrix() * sceneData.view->getCameraMatrix() *
                               drawable->getModelMatrix());
        uniform("normalMatrix", drawable->getNormalMatrix());
        uniform("modelMatrix", drawable->getModelMatrix());
        uniform("tilingSize", drawable->getScale().x / 4.0f);
    }
};

class GuiShader : public Shader {
public:
    GuiShader(std::vector<const char *> &shaderFiles, ShaderType type) : Shader(shaderFiles, type) {}
    void setSceneUniforms(SceneRenderData &sceneData, void *data) {}
    void setDrawableUniforms(SceneRenderData &sceneData, Drawable *drawable, void *data) {
        uniform("MVP", sceneData.view->getOrthoProjection() * drawable->getModelMatrix());
    }
};

class PostProcessingShader : public Shader {
public:
    PostProcessingShader(std::vector<const char *> &shaderFiles, ShaderType type) : Shader(shaderFiles, type) {}
    void setSceneUniforms(SceneRenderData &sceneData, void *data) {
        TerrainObjectAttributes *attribs = reinterpret_cast<TerrainObjectAttributes *>(data);

        uniform("cameraMatrix", sceneData.view->getCameraMatrix());
        uniform("projectionMatrix", sceneData.view->getProjectionMatrix());
        uniform("sunPosition", sceneData.sun->getPosition());
        uniform("camDirection", sceneData.view->getCameraDirection());
        uniform("worldSpaceCamPos", sceneData.view->getCameraPosition());
        uniform("nearPlane", sceneData.view->getNearPlane());
        uniform("farPlane", sceneData.view->getFarPlane());

        uniform("planetOrigin", attribs->bodyOrigin);
        uniform("planetRadius", attribs->bodyRadius);
    }
    void setDrawableUniforms(SceneRenderData &sceneData, Drawable *drawable, void *data) {
        uniform("MVP", sceneData.view->getProjectionMatrix() * sceneData.view->getCameraMatrix() *
                               drawable->getModelMatrix());
        uniform("modelMatrix", drawable->getModelMatrix());
        uniform("positionOriginal", drawable->getPosition());
    }
};

class DebugShader : public Shader {
public:
    DebugShader(std::vector<const char *> &shaderFiles, ShaderType type) : Shader(shaderFiles, type) {}
    void setSceneUniforms(SceneRenderData &sceneData, void *data) {
        uniform("cameraMatrix", sceneData.view->getCameraMatrix());
        uniform("projectionMatrix", sceneData.view->getProjectionMatrix());
    }
    void setDrawableUniforms(SceneRenderData &sceneData, Drawable *drawable, void *data) {
        uniform("modelMatrix", drawable->getModelMatrix());
    }
};

class TessellationShader : public Shader {
public:
    TessellationShader(std::vector<const char *> &shaderFiles, ShaderType type) : Shader(shaderFiles, type) {}
    void setSceneUniforms(SceneRenderData &sceneData, void *data) {
        Light *light = dynamic_cast<Light *>(sceneData.sun);

        uniform("cameraMatrix", sceneData.view->getCameraMatrix());
        uniform("cameraPos", sceneData.view->getCameraPosition());
        uniform("projectionMatrix", sceneData.view->getProjectionMatrix());
        uniform("light.position", light->getPosition());
        uniform("light.color", light->getColor());
        uniform("light.ambient", light->getAmbient());
        uniform("light.diffuse", light->getDiffuse());
        uniform("light.specular", light->getSpecular());
        uniform("frameTime", g_currentFrameTime);
    }
    void setDrawableUniforms(SceneRenderData &sceneData, Drawable *drawable, void *data) {
        uniform("modelMatrix", drawable->getModelMatrix());
    }
};

class SkyBoxShader : public Shader {
public:
    SkyBoxShader(std::vector<const char *> &shaderFiles, ShaderType type) : Shader(shaderFiles, type) {}
    void setSceneUniforms(SceneRenderData &sceneData, void *data) {}
    void setDrawableUniforms(SceneRenderData &sceneData, Drawable *drawable, void *data) {
        glm::mat4 mvp = sceneData.view->getProjectionMatrix() * glm::mat4(glm::mat3(sceneData.view->getCameraMatrix())) *
                    drawable->getModelMatrix();

        uniform("modelMatrix", drawable->getModelMatrix());
        uniform("MVP", mvp);
    }
};

class SunShader : public Shader {
public:
    SunShader(std::vector<const char *> &shaderFiles, ShaderType type) : Shader(shaderFiles, type) {}
    void setSceneUniforms(SceneRenderData &sceneData, void *data) {

    }
    void setDrawableUniforms(SceneRenderData &sceneData, Drawable *drawable, void *data) {
        Light *light = dynamic_cast<Light *>(drawable);
        uniform("color", light->getColor());
        uniform("MVP", sceneData.view->getProjectionMatrix() * sceneData.view->getCameraMatrix() *
                               drawable->getModelMatrix());
    }
};
#endif