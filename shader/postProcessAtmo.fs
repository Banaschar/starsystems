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

float linearDepth(float depth) {
    return 2.0 * 0.1 * 2000.0 / (2000 + 0.1 - (2.0 * depth - 1.0) * (2000 - 0.1));
}

void main() {
    vec4 screen = texture(mainScreenTex, texCoords);
    float depth = texture(mainDepthTexture, texCoords).r;
    float eyeDepth = linearDepth(depth) * length(camDirection);

    vec3 rayDirection = normalize(camDirection);
    vec3 vPos = worldPosition.xyz;
    // the real way: translating the point y units in the direction of the cameraUp vector
    // And then x units in the direction of a vector perpendicular to the cameraUp and the camera Direction vector
    // Just try when I need the negative result of the cross product
    float mX = gl_FragCoord.x > 640 ? gl_FragCoord.x - 640 : 0 - (640 - gl_FragCoord.x);
    float mY = gl_FragCoord.y > 360 ? gl_FragCoord.y - 360 : 0 - (360 - gl_FragCoord.y);
    vPos = vPos + mY * normalize(-cameraUp);
    vPos = vPos + mX * normalize(-cameraRight);

    //vPos.x -= gl_FragCoord.x > 640 ? gl_FragCoord.x - 640 : 0 - (640 - gl_FragCoord.x);
    //vPos.y += gl_FragCoord.y > 360 ? gl_FragCoord.y - 360 : 0 - (360 - gl_FragCoord.y);
    vec2 intersect = intersectRaySphere(planetOrigin, planetRadius + atmosphereHeight_, vPos, rayDirection);
    float dstToAtmosphere = intersect.x;
    float dstThroughAtmosphere = min(intersect.y, eyeDepth - dstToAtmosphere);
    float p = dstThroughAtmosphere / (planetRadius * 2);
    

    //color = 1 - screen;
    //color = screen;
    color = vec4(p,p,p,p);
    //color = vec4(1,0,0,0);

}