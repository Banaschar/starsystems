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
uniform sampler2D texture_height1;
uniform sampler2D texture_normal1;
uniform vec3 camPos;
uniform float heightMapDimension;
uniform float meshDimension;
uniform vec3 gridOrigin;
uniform float lowerBound;
uniform float upperBound;

uniform vec3 cameraPos;
uniform mat4 cameraMatrix;

float getMorphFactor(float dist, float low, float high) {
    float delta = high - low;
    float factor = (dist - low) / delta;
    return clamp(factor / 0.7 - 1.0, 0.0, 1.0);
}

/*
 * WorldPos: World Coordinates of the vertex
 * MeshPos: Position in the mesh grid.
 * The frac part only decides if we morph or not, 1 or 0.  
 */
vec2 morphVertex(vec2 worldPos, vec2 meshPos, float morphFactor, float scale) {
    vec2 meshDim = vec2(meshDimension, meshDimension);
    vec2 normMeshPos = meshPos / meshDimension;
    vec2 frac = fract(normMeshPos * meshDim * 0.5) * 2.0;
    return worldPos - frac * scale * morphFactor;
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
    float prevRange = rangeScale.y;
    float scale = rangeScale.z;

    vec4 vWorldPos = aInstanceModelMat * vec4(vertexPosition, 1.0);
    
    float h = decodeHeightRange(texture(texture_height1, getNormalizedGridCoord(vWorldPos.xz)).r);
    vWorldPos.y = h;
    
    float dist = distance(cameraPos, vWorldPos.xyz);
    //float d = range + 0.6 * (nextRange - range);
    //float morphFactor = 1.0f - clamp(d - dist, 0.0, 1.0);
    float morphFactor = getMorphFactor(dist, prevRange, range);
    
    if (morphFactor == 1.0)
        fColor = vec4(1.0,0.0,0.0,1.0);
    else if (morphFactor > 0.5)
        fColor = vec4(0.0,1.0,0.0,1.0);
    else
        fColor = vec4(1.0,1.0,1.0,1.0);
    
    vec2 morphed = morphVertex(vWorldPos.xz, vertexPosition.xz, morphFactor, scale);

    h = decodeHeightRange(texture(texture_height1, getNormalizedGridCoord(morphed)).r);
    vec4 vWorldPosMorphed = vec4(morphed.x, h, morphed.y, 1.0);

    // normals
    vec3 n = texture(texture_normal1, getNormalizedGridCoord(vWorldPosMorphed.xz)).rgb;
    vec3 normal = vec3(decodeNormalRange(n.r), decodeNormalRange(n.g), decodeNormalRange(n.b));
    
    gl_Position = VP * vWorldPosMorphed;
    wPos_frag_in = vWorldPosMorphed.xyz;
    normal_frag_in = normal;

    // Debug normal stuff:
    //gl_Position = cameraMatrix * vPos;
    //mat3 normalMatrix = mat3(transpose(inverse(cameraMatrix * aInstanceModelMat)));
    //vs_out.normal = normalize(vec3(vec4(normalMatrix * normal, 1.0)));
} 