#version 410 core
layout (location = 0) in vec3 vertexPosition;
layout (location = 2) in vec2 textureCoords;

out vec4 fColor;
out vec3 normal_frag_in;
out vec3 wPos_frag_in;

uniform mat4 VP;
uniform mat4 modelMatrix;
uniform sampler2D texture_heightMap1;
uniform sampler2D texture_normalMap1;
uniform float range;
uniform vec3 camPos;
uniform float gridDimension;
uniform vec3 gridOrigin;
uniform float lowerBound;
uniform float upperBound;

vec2 morphVertex(vec2 gridPos, vec2 worldPos, float morph) {
    vec2 gridDim = vec2(gridDimension, gridDimension);
    vec2 fracPart = fract(gridPos * gridDim * 0.5) * 2.0 / gridDim.xy;
    return worldPos - fracPart * morph;
}

float decodeHeightRange(float inVal) {
    return ((inVal - 0.0) / (1.0 - 0.0)) * (upperBound - lowerBound) + lowerBound;
}

float decodeNormalRange(float inVal) {
    return ((inVal + 0.0) / (1.0 - 0.0)) * (1.0 + 1.0) - 1.0;
}

vec2 getNormalizedGridCoord(vec2 worldPos) {
    //vec2 p = abs(worldPos - gridOrigin.xz);
    return worldPos / gridDimension;
}

void main()
{
    vec4 vPos = modelMatrix * vec4(vertexPosition, 1.0);
    float h = decodeHeightRange(texture(texture_heightMap1, getNormalizedGridCoord(vPos.xz)).r);
    vPos.y = h;
    vec3 n = texture(texture_normalMap1, getNormalizedGridCoord(vPos.xz)).rgb;
    vec3 normal = vec3(decodeNormalRange(n.r), decodeNormalRange(n.g), decodeNormalRange(n.b));

    gl_Position = VP * vPos;
    wPos_frag_in = vPos.xyz;
    normal_frag_in = normal;
} 