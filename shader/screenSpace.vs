#version 330 core
layout (location = 0) in vec3 vertexPosition;
layout (location = 2) in vec2 textureCoords;

out vec2 texCoords;
out vec4 near_4;
out vec4 far_4;

uniform mat4 projectionMatrix;
uniform mat4 cameraMatrix;

void main()
{
    gl_Position = vec4(vertexPosition, 1.0);

    mat4 VP = projectionMatrix * cameraMatrix;
    mat4 inverseVP = inverse(VP);
    near_4 = inverseVP * vec4(vertexPosition.x, vertexPosition.y, -1.0, 1.0);
    far_4 = inverseVP * vec4(vertexPosition.x, vertexPosition.y, 1.0, 1.0);
    texCoords = textureCoords;
}