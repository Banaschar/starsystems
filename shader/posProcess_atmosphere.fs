#version 330 core

in vec2 TexCoords; // TexCoords for the main output texture

out vec4 FragColor;
/*
uniform sampler2D mainScreenTex; // Main rendered texture 

uniform vec3 worldSpaceCamPos;
uniform vec3 camDirection;
uniform vec3 planetOrigin;
uniform float planetRadius;

float atmosphereHeight_ = 80.0;

vec2 intersectRaySphere(vec3 sphereCenter, float sphereRadius, float rayOrigin, float rayDirection) {
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
    return vec2(-1,-1);
}
*/
void main() {
    //vec3 rayDirection = normalize(camDirection);

    //vec2 intersect = intersectRaySphere(planetOrigin, planetRadius + atmosphereHeight_, worldSpaceCamPos, rayDirection);
    //float dstToAtmosphere = intersect.x;
    //float dstTThroughAtmosphere = intersect.y;

    //vec4 screen = texture(mainScreenTex, TexCoords);
    //return 1 - screen;
    FragColor = vec4(1,0,0,1);
}