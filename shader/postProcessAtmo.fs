#version 330 core

in vec2 texCoords;
in vec4 worldPosition;
in vec3 cameraUp;
in vec3 cameraRight;

out vec4 color;

uniform sampler2D mainScreenTex;
uniform sampler2D mainDepthTexture;

uniform vec3 camDirection;
uniform vec3 planetOrigin;
uniform int planetRadius;
uniform mat4 camMatrix;
uniform mat4 projectionMatrix;
uniform mat4 cameraMatrix;
uniform float farPlane;
uniform float nearPlane;
uniform vec3 worldSpaceCamPos;

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
    return vec2(1000000000, 0);
}

vec3 worldPosFromDepth(float depth) {
    float z = depth * 2.0 - 1.0;

    vec4 clipSpacePosition = vec4(texCoords * 2.0 - 1.0, z, 1.0);
    vec4 viewSpacePosition = inverse(projectionMatrix) * clipSpacePosition;

    // Perspective division
    viewSpacePosition /= viewSpacePosition.w;

    vec4 worldSpacePosition = inverse(cameraMatrix) * viewSpacePosition;

    return worldSpacePosition.xyz;
}

/* linearized depth: depth is [0,1] but not linear by default (So it has higher floating point accuracy close to the camera,
 * but lower accuracy farther away)
 */
float linearDepth(float depth) {
    // (2.0 * depth - 1.0) -> Normalized device coordinates (mapped between [-1,1])
    return 2.0 * 0.1 * 2000.0 / (2000 + 0.1 - (2.0 * depth - 1.0) * (2000 - 0.1));
}

void main() {
    vec4 screen = texture(mainScreenTex, texCoords);
    float depth = texture(mainDepthTexture, texCoords).r; // scale texCoords?????
    float eyeDepth = linearDepth(depth) * length(camDirection);

    vec3 rayDirection = normalize(camDirection);
    vec3 vPos = worldPosition.xyz;
    // the real way: translating the point y units in the direction of the cameraUp vector
    // And then x units in the direction of a vector perpendicular to the cameraUp and the camera Direction vector
    // Just try when I need the negative result of the cross product
    /*
    float mX = gl_FragCoord.x > 640 ? gl_FragCoord.x - 640 : 0 - (640 - gl_FragCoord.x);
    float mY = gl_FragCoord.y > 360 ? gl_FragCoord.y - 360 : 0 - (360 - gl_FragCoord.y);
    vPos = vPos + mY * normalize(cameraUp);
    vPos = vPos + mX * normalize(cameraRight);
    */
    vec2 intersect = intersectRaySphere(planetOrigin, planetRadius + atmosphereHeight_, vPos, rayDirection);

    // need to map worldPosFromDepth(depth) on the plane P(camPos, camNormalVector);
    /*
    vec3 wPosdepth = worldPosFromDepth(depth);
    vec3 projectedOnCamPos = wPosdepth - dot(wPosdepth - worldSpaceCamPos, camDirection) * camDirection; 
    vec2 intersect = intersectRaySphere(planetOrigin, planetRadius + atmosphereHeight_, projectedOnCamPos, rayDirection);
    */

    float dstToAtmosphere = intersect.x;
    float dstThroughAtmosphere = intersect.y;
    //float dstThroughAtmosphere = min(intersect.y, eyeDepth - dstToAtmosphere);
    float atmo = dstThroughAtmosphere; // (planetRadius * 2);
    float planet = eyeDepth / 2000;//distance(planetOrigin, worldSpaceCamPos);
    //float planet = eyeDepth - distance(planetOrigin, worldSpaceCamPos);
    //float c = min(atmo, planet);

    //color = vec4(c,c,c,1.0);
    //color = 1 - screen;
    //color = screen;
    color = vec4(atmo,atmo,atmo,1.0);
    //color = vec4(planet,planet,planet,1.0); // DEPTH MAP IS CORRECT
}