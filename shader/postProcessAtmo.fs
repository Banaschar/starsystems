#version 330 core

in vec2 texCoords;

out vec4 color;

uniform sampler2D mainScreenTex;

uniform vec3 worldSpaceCamPos;
uniform vec3 camDirection;
uniform vec3 planetOrigin;
uniform int planetRadius;
uniform mat4 camMatrix;
uniform mat4 projectionMatrix;

float atmosphereHeight_ = 0.0;

vec2 intersectRaySphere(vec3 sphereCenter, float sphereRadius, vec3 rayOrigin, vec3 rayDirection) {
    vec3 off = rayOrigin - sphereCenter;
    float a = 1;
    float b = 2 * dot(off, rayDirection);
    float c = dot(off, off) - sphereRadius * sphereRadius;
    float d = b * b - 4 * a * c; // Quadratic formula root part

    // num intersections
    if (d > 0) {
        float s = sqrt(d);
        float dstToSphereNear = max(0, (-b - s) / (2 * a));
        float dstToSphereFar = (-b + s) / (2 * a);

        if (dstToSphereFar >= 0) {
            return vec2(dstToSphereNear, dstToSphereFar - dstToSphereNear);
        }
    }

    // No intersection
    return vec2(100000000, 0);
}

void main() {
    // Try to get frag pos in world space
    
    vec4 viewport = vec4(0,0,1280,720);
    vec4 ndcPos;
    ndcPos.xy = ((2.0 * gl_FragCoord.xy) - (2.0 * viewport.xy)) / (viewport.zw) - 1;
    ndcPos.z = (2.0 * gl_FragCoord.z - gl_DepthRange.near - gl_DepthRange.far) /
    (gl_DepthRange.far - gl_DepthRange.near);
    ndcPos.w = 1.0;

    //vec4 clipPos = ndcPos / gl_FragCoord.w;
    //vec4 eyePos = inverse(projectionMatrix) * clipPos;
    


    //vec4 camPos = camMatrix * vec4(worldSpaceCamPos, 1);
    vec3 camPos = worldSpaceCamPos;
    vec3 rayDirection = normalize(camDirection);
    vec2 intersect = intersectRaySphere(planetOrigin, planetRadius + atmosphereHeight_, ndcPos.xyz, rayDirection);
    float dstToAtmosphere = intersect.x;
    float dstThroughAtmosphere = intersect.y;

    vec4 screen = texture(mainScreenTex, texCoords);
    float p = dstThroughAtmosphere / (planetRadius * 2);
    
    //color = 1 - screen;
    color = screen;
    //color = vec4(p,0,0,1);
    /*
    if (p > 1)
        color = vec4(1,0,0,1);
    else
        color = vec4(0,1,0,1);
    */
}