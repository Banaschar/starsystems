#version 330 core
layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in vec2 textureCoords;
layout (location = 3) in vec4 vertexColor;

out vec4 color;
out vec3 fragPos_worldspace;
out vec3 normal;
out vec2 texCoords;

uniform mat4 modelMatrix;
uniform mat3 normalMatrix;
uniform mat4 MVP;
uniform vec4 clipPlane;
uniform float tiling;

void main()
{
    color = vertexColor;
    //normal = normalMatrix * vertexNormal;
    normal = vertexNormal;
    vec4 modelPos = modelMatrix * vec4(vertexPosition, 1.0);
    fragPos_worldspace = vertexPosition;//modelPos.xyz;
    texCoords = textureCoords * tiling; // Tile texture over terrain
    gl_ClipDistance[0] = dot(modelPos, clipPlane); // water render clipping
    gl_Position = MVP * vec4(vertexPosition, 1.0);
} 