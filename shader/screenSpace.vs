#version 330 core
layout (location = 0) in vec3 vertexPosition;
layout (location = 2) in vec2 textureCoords;

out vec2 texCoords;
out vec4 worldPosition;
out vec3 cameraUp;
out vec3 cameraRight;

uniform mat4 modelMatrix;
uniform mat4 projectionMatrix;
uniform mat4 cameraMatrix;
uniform mat4 MVP;
uniform vec3 size;
uniform vec3 worldSpaceCamPos;
uniform vec3 positionOriginal;

void main()
{
    gl_Position = vec4(vertexPosition, 1.0);
    
    // Billboard positioning
    // http://www.opengl-tutorial.org/intermediate-tutorials/billboards-particles/billboards/
    vec3 camRight = vec3(cameraMatrix[0][0], cameraMatrix[1][0], cameraMatrix[2][0]);
    vec3 camUp = vec3(cameraMatrix[0][1], cameraMatrix[1][1], cameraMatrix[2][1]);

    mat4 VP = projectionMatrix * cameraMatrix;
    vec4 final_world = vec4(positionOriginal + camRight * vertexPosition.x * size.x + camUp * vertexPosition.y * size.y, 1.0);

    //gl_Position = VP * final_world;
    texCoords = textureCoords;
    worldPosition = final_world;
    cameraUp = camUp;
    cameraRight = camRight;
}