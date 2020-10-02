#version 410 core
layout (location = 0) in vec3 vertexPosition;
layout (location = 3) in mat4 aInstanceModelMat;
layout (location = 7) in vec3 rangeScale;

out vec4 fColor;
out vec3 normal_frag_in;
out vec3 wPos_frag_in;

out VS_OUT {
    vec3 normal;
} vs_out;

uniform mat4 VP;
uniform mat4 modelMatrix;
uniform sampler2D texture_height1;
uniform sampler2D texture_normal1;
uniform vec3 camPos;
uniform float heightMapDimension;
uniform float gridDimension;
uniform vec3 gridOrigin;
uniform float lowerBound;
uniform float upperBound;

uniform vec3 cameraPos;
uniform mat4 cameraMatrix;
uniform mat4 projectionMatrix;

vec2 morphVertex(vec2 gridPos, vec2 worldPos, float morph, float scale) {
    vec2 gridDim = vec2(gridDimension, gridDimension);
    vec2 fracPart = fract(gridPos * gridDim * 0.5) * 2.0 / gridDim.xy;
    return worldPos - fracPart * scale * morph;
}

float decodeHeightRange(float inVal) {
    return ((inVal - 0.0) / (1.0 - 0.0)) * (upperBound - lowerBound) + lowerBound;
}

float decodeNormalRange(float inVal) {
    return inVal * 2.0 - 1.0;
}

vec2 getNormalizedGridCoord(vec2 worldPos) {
    //vec2 p = abs(worldPos - gridOrigin.xz);
    return worldPos / heightMapDimension;
}

void main()
{
    float morphStart = 0.0;
    float morphEnd = 1.0;
    float range = rangeScale.x;
    float nextRange = rangeScale.y;
    float scale = rangeScale.z;

    vec4 vWorldPos = aInstanceModelMat * vec4(vertexPosition, 1.0);
    
    float h = decodeHeightRange(texture(texture_height1, getNormalizedGridCoord(vWorldPos.xz)).r);
    vWorldPos.y = h;
    
    float dist = distance(cameraPos, vWorldPos.xyz);
    //float nextlevel_threshold = ((range - dist) / scale * gridDimension / 16.0);
    //float rangeDist = 1.0 - smoothstep(morphStart, morphEnd, nextlevel_threshold);
    float d = range + 0.6 * (nextRange - range);
    float morphFactor = 1.0f - clamp(d - dist, 0.0, 1.0);

    vec4 vWorldPosMorphed = vec4(0.0, 0.0, 0.0, 1.0);
    vWorldPosMorphed.xz = morphVertex(getNormalizedGridCoord(vWorldPos.xz), vWorldPos.xz, morphFactor, scale);
    vWorldPosMorphed.y = h;

    // normals
    vec3 n = texture(texture_normal1, getNormalizedGridCoord(vWorldPos.xz)).rgb;
    vec3 normal = vec3(decodeNormalRange(n.r), decodeNormalRange(n.g), decodeNormalRange(n.b));
    
    gl_Position = VP * vWorldPosMorphed;
    wPos_frag_in = vWorldPosMorphed.xyz;
    normal_frag_in = normal;
    
    //gl_Position = VP * vWorldPos;

    // Debug normal stuff:
    //gl_Position = cameraMatrix * vPos;
    //mat3 normalMatrix = mat3(transpose(inverse(cameraMatrix * modelMatrix)));
    //vs_out.normal = normalize(vec3(vec4(normalMatrix * normal, 1.0)));
} 