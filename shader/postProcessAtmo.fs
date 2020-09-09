#version 330 core

in vec2 texCoords;
in vec4 near_4;
in vec4 far_4;

out vec4 color;

uniform sampler2D mainScreenTex;
uniform sampler2D mainDepthTexture;

uniform vec3 sunPosition;
uniform vec3 camDirection;
uniform vec3 planetOrigin;
uniform int planetRadius;
uniform float farPlane;
uniform float nearPlane;
uniform vec3 worldSpaceCamPos;
uniform vec3 scatterCoeffs;

float atmosphereHeight_ = 100.0;
float numScatteringPoints_ = 15.0;
float numOpticalDepthPoints_ = 15.0;
float densityFalloff_ = 6.18;

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
    return 2.0 * nearPlane * farPlane / (nearPlane + farPlane - (2.0 * depth - 1.0) * (farPlane - nearPlane));
}

float densityAtPoint(vec3 densitySamplePoint) {
    float heightAboveSurface = length(densitySamplePoint - planetOrigin) - planetRadius;
    float height = heightAboveSurface / atmosphereHeight_;
    float localDensity = exp(-height * densityFalloff_) * (1 - height);

    return localDensity;
}

float opticalDepth(vec3 rayOrigin, vec3 rayDir, float rayLength) {
    vec3 densitySamplePoint = rayOrigin;
    float stepSize = rayLength / (numOpticalDepthPoints_ - 1);
    float opticalDepth = 0;

    for (int i = 0; i < numOpticalDepthPoints_; ++i) {
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
    for (int i = 0; i < numScatteringPoints_; ++i) {
        vec3 dirToSun = normalize(sunPosition - scatterPoint);
        float sunRayLength = intersectRaySphere(planetOrigin, planetRadius + atmosphereHeight_, scatterPoint, dirToSun).y;
        float sunRayOpticalDepth = opticalDepth(scatterPoint, -dirToSun, sunRayLength);
        viewRayOpticalDepth = opticalDepth(scatterPoint, -rayDir, stepSize * i);
        // The higher the density (optical depth) of the atmoshpere the lower the light that gets through. Gets lower on a negative e^density function
        vec3 transmittance = exp(-(sunRayOpticalDepth + viewRayOpticalDepth) * scatterCoeffs);
        float localDensity = densityAtPoint(scatterPoint);

        scatteredLight += localDensity * transmittance * scatterCoeffs * stepSize;
        scatterPoint += rayDir * stepSize;
    }

    float originalColorTransmittance = exp(-viewRayOpticalDepth);
    return originalColors * originalColorTransmittance + scatteredLight;
}

void main() {
    vec4 screen = texture(mainScreenTex, texCoords);
    float depth = texture(mainDepthTexture, texCoords).r; // scale texCoords?????
    float eyeDepth = linearDepth(depth) * length(camDirection);

    /* AND ANOTHER ! BEST SO FAR !*/
    vec3 origin = near_4.xyz / near_4.w;
    vec3 direction = (far_4.xyz / far_4.w) - origin;
    //float eyeDepth = linearDepth(depth) * length(direction);
    direction = normalize(direction);

    
    vec2 intersect = intersectRaySphere(planetOrigin, planetRadius + atmosphereHeight_, origin, direction);
    float dstToAtmosphere = intersect.x;
    //float dstThroughAtmosphere = intersect.y;
    float dstThroughAtmosphere = min(intersect.y, eyeDepth - dstToAtmosphere);

    /*
    float atmo = dstThroughAtmosphere / (planetRadius * 2);
    //float planet = eyeDepth / distance(planetOrigin, worldSpaceCamPos);
    //float planet = eyeDepth - distance(planetOrigin, worldSpaceCamPos);
    //float c = min(atmo, planet);

    //color = vec4(c,c,c,1.0);
    //color = 1 - screen;
    //color = screen;
    //color = vec4(atmo,atmo,atmo,1.0);
    //color = vec4(planet,planet,planet,1.0); // DEPTH MAP IS CORRECT
    */

    if (dstThroughAtmosphere > 0) {
        const float epsilon = 0.0001; // floating point precision offset
        vec3 pointInAtmosphere = origin + (dstToAtmosphere + epsilon) * direction;
        vec3 light = calculateLight(pointInAtmosphere, direction, dstThroughAtmosphere - epsilon * 2, screen.xyz);
        //color = screen * (1 - light) + light;
        color = vec4(light, 0.0);
    } else {
        color = screen;
    }
}