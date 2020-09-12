#version 330 core

in vec2 texCoords;
in vec4 near_4;
in vec4 far_4;

out vec4 color;

uniform sampler2D mainScreenTex;
uniform sampler2D mainDepthTexture;

uniform mat4 projectionMatrix;
uniform mat4 cameraMatrix;

uniform vec3 sunPosition;
uniform vec3 camDirection;
uniform vec3 planetOrigin;
uniform int planetRadius;
uniform float farPlane;
uniform float nearPlane;
uniform vec3 worldSpaceCamPos;
uniform vec3 scatterCoeffs;

float atmosphereHeight_ = planetRadius / 4.0;
//float atmosphereHeight_ = 250.0;
float numScatteringPoints_ = 10.0;
float numOpticalDepthPoints_ = 10.0;
float densityFalloff_ = 24.18;
//float densityFalloff_ = 15.1;

int testBool = 0;

vec3 worldPosFromDepth(float depth) {
    float z = depth * 2.0 - 1.0;

    vec4 clipSpacePosition = vec4(texCoords * 2.0 - 1.0, z, 1.0);
    vec4 viewSpacePosition = inverse(projectionMatrix) * clipSpacePosition;

    // Perspective division
    viewSpacePosition /= viewSpacePosition.w;

    vec4 worldSpacePosition = inverse(cameraMatrix) * viewSpacePosition;

    return worldSpacePosition.xyz;
}

vec3 worldPosFromDepth2(float depth) {
    float z = depth * 2.0 - 1.0;
    vec4 clip = vec4(texCoords * 2.0 - 1.0, z, 1.0);
    clip = inverse(projectionMatrix * cameraMatrix) * clip;

    return clip.xyz / clip.w;
}

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

/* linearized depth: depth is [0,1] but not linear by default (So it has higher floating point accuracy close to the camera,
 * but lower accuracy farther away)
 */
float linearDepth(float depth) {
    // (2.0 * depth - 1.0) -> Normalized device coordinates (mapped between [-1,1])
    //return 2.0 * 0.1 * 2000.0 / (2000 + 0.1 - (2.0 * depth - 1.0) * (2000 - 0.1));
    return 2.0 * nearPlane * farPlane / (farPlane + nearPlane - (2.0 * depth - 1.0) * (farPlane - nearPlane));
}

float densityAtPoint(vec3 densitySamplePoint) {
    float heightAboveSurface = distance(densitySamplePoint, planetOrigin) - planetRadius;
    float height = heightAboveSurface / atmosphereHeight_;
    float localDensity = exp(-height * densityFalloff_);// * (1 - height);

    return localDensity;
}

float opticalDepth(vec3 rayOrigin, vec3 rayDir, float rayLength) {
    vec3 densitySamplePoint = rayOrigin;
    float stepSize = rayLength / (numOpticalDepthPoints_ - 1);
    float opticalDepth = 0;

    for (int i = 0; i < numOpticalDepthPoints_; i++) {
        float localDensity = densityAtPoint(densitySamplePoint);
        opticalDepth += localDensity * stepSize;
        densitySamplePoint += rayDir * stepSize;
    }

    return opticalDepth;
}

/*
 * Optical Depth is the density at a point in the atmosphere
 */
vec3 calculateLight(vec3 rayOrigin, vec3 rayDir, float rayLength, vec3 originalColors) {
    vec3 scatterPoint = rayOrigin;
    float stepSize = rayLength / (numScatteringPoints_ - 1.0);
    vec3 scatteredLight = vec3(0.0);
    float viewRayOpticalDepth = 0.0;
    float sunRayOpticalDepth = 0.0;
    for (int i = 0; i < numScatteringPoints_; i++) {
        vec3 dirToSun = normalize(sunPosition - scatterPoint);
        float sunRayLength = intersectRaySphere(planetOrigin, planetRadius + atmosphereHeight_, scatterPoint, dirToSun).y;
        sunRayOpticalDepth = opticalDepth(scatterPoint, dirToSun, sunRayLength);
        viewRayOpticalDepth = opticalDepth(scatterPoint, -rayDir, stepSize * i); 
        // The higher the density (optical depth) of the atmoshpere the lower the light that gets through. Gets lower on a negative e^density function
        vec3 transmittance = exp(-(sunRayOpticalDepth + viewRayOpticalDepth) * scatterCoeffs);
        float localDensity = densityAtPoint(scatterPoint);

        scatteredLight += localDensity * transmittance * scatterCoeffs * stepSize;
        scatterPoint += rayDir * stepSize;
    }

    float originalColorTransmittance = exp(-viewRayOpticalDepth);
    return originalColors * (1 - scatteredLight) + scatteredLight; // Surface visible...but too much?

    //return originalColors * originalColorTransmittance + scatteredLight; // Original - but everything is dark
}

void main() {
    vec4 screen = texture(mainScreenTex, texCoords);
    float depth = texture(mainDepthTexture, texCoords).r; // scale texCoords?????
    //float linearEyeDepth = linearDepth(depth);// * length(camDirection);
    vec3 worldDepthPos = worldPosFromDepth(depth);

    vec3 origin = near_4.xyz / near_4.w;
    vec3 direction = (far_4.xyz / far_4.w) - origin;
    //float linearEyeDepth = linearDepth(depth) * length(direction);
    direction = normalize(direction);
    float eyeDepth = length(worldDepthPos - origin);
    
    
    vec2 intersect = intersectRaySphere(planetOrigin, planetRadius + atmosphereHeight_, origin, direction);
    float dstToAtmosphere = intersect.x;
    float dstThroughAtmosphere = min(intersect.y, eyeDepth - dstToAtmosphere);

    if (dstThroughAtmosphere > 0) {
        const float epsilon = 0.0001; // floating point precision offset
        vec3 pointInAtmosphere = origin + (dstToAtmosphere + epsilon) * direction;
        vec3 light = calculateLight(pointInAtmosphere, direction, dstThroughAtmosphere - epsilon * 2, screen.xyz);
        color = vec4(light, 0.0);
    } else {
        color = screen;
    }
}