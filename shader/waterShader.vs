#version 330 core
layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in vec2 textureCoords;
layout (location = 3) in vec4 vertexColor;

out vec3 fragPos_worldspace;
out vec4 color;
out vec3 normal;
out vec2 texCoords;
out vec4 clipSpace;

uniform mat4 MVP;
uniform mat4 modelMatrix;
uniform mat3 normalMatrix;
uniform float tilingSize;

const float DEFAULT_TILING_SIZE = 20.0;

void main() {
    clipSpace = MVP * vec4(vertexPosition, 1.0);
    gl_Position = clipSpace;
    float tiling;
    if (tilingSize > 0) 
        tiling = tilingSize;
    else 
        tiling = DEFAULT_TILING_SIZE;
    texCoords = vec2(vertexPosition.x / 2.0 + 0.5, vertexPosition.z / 2.0 + 0.5) * tiling;
    color = vertexColor;
    fragPos_worldspace = vec3(modelMatrix * vec4(vertexPosition, 1.0));
    normal = normalMatrix * vertexNormal;
}