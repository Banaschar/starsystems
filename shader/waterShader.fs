#version 330 core

out vec4 fragColor;

struct Light {
    vec3 direction;
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 color;
};

in vec4 color;
in vec3 fragPos_worldspace;    // model matrix * vertexPosition
in vec3 normal;     // Normal matrix * vertexNormal
in vec2 texCoords;  // tex coords from vertex position in normalized device space, /2 + 0.5 to move the origin to the lower left corner
/* 
Normalized device space  |  normalized device space /2 + 0.5
     1                      1
     |                      |
-1---0---1                  |
     |                      |
    -1                      0-------1
*/
in vec4 clipSpace; // vertex position in clip space

uniform Light light;
uniform vec3 worldNormal;
uniform vec3 cameraPos;

uniform sampler2D texture_reflection;
uniform sampler2D texture_refraction;
uniform sampler2D texture_dudv;
uniform sampler2D texture_normal;
uniform sampler2D texture_depth;

uniform float moveFactor;
uniform float farPlane;
uniform float nearPlane;

const float waveStrength = 0.05;
const float shineDamper = 64.0;
const float reflectivity = 0.6;

vec4 calculateLighting() {
    // ambient
    vec3 ambient = light.ambient * 0.1;

    // diffuse
    vec3 lightDir = normalize(-light.direction);
    vec3 norm = normalize(normal);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff;

    //specular
    vec3 viewDirection = normalize(cameraPos - fragPos_worldspace);
    vec3 reflectDirection = reflect(-light.direction, norm);
    float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), 32.0);
    vec3 specular = 0.5 * spec * light.specular;

    return vec4(ambient + diffuse + specular, 1.0);
}

/*
 * Everythin from the "Refraction depth map part delivers sub optimal results"
 * -> waterDepth is used 3 times, for distortion, specular, and alpha at the end.
 * ----> remove maybe, if it's not better with better underwater textures.
 *
 * ---> USE waterDepth to generate like white foam from waves at the edges
 *
 * ---> REDUCE specular highlight when moving the camera farther away, like really far torwards orbit
 *
 * --> My water consits of 6 vertices -> vertex shader is only run 6 times. Move more code to the vertex shader!!!!!!!
 */
vec4 reflectRefract() {
    vec2 ndc = (clipSpace.xy / clipSpace.w); // normalized device coordinates
    ndc = ndc / 2.0 + 0.5; 
    vec2 refractTexCoords = vec2(ndc.x, ndc.y);
    vec2 reflectTexCoords = vec2(ndc.x, -ndc.y);

    // Refraction depth map
    float depth = texture(texture_depth, refractTexCoords).r;
    float floorDistance = 2.0 * nearPlane * farPlane / (farPlane + nearPlane - (2.0 * depth - 1.0) * (farPlane - nearPlane));
    
    depth = gl_FragCoord.z;
    float waterDistance = 2.0 * nearPlane * farPlane / (farPlane + nearPlane - (2.0 * depth - 1.0) * (farPlane - nearPlane));
    float waterDepth = floorDistance - waterDistance;

    // Distortion
    vec2 distortionTexCoords = texture(texture_dudv, vec2(texCoords.x + moveFactor, texCoords.y)).xy * 0.1;
    distortionTexCoords = texCoords + vec2(distortionTexCoords.x, distortionTexCoords.y + moveFactor);
    vec2 distortion = (texture(texture_dudv, distortionTexCoords).xy * 2.0 - 1.0) * waveStrength * clamp(waterDepth/5.0, 0.0, 1.0); // * clamp is questionable.

    reflectTexCoords += distortion;
    reflectTexCoords.x = clamp(reflectTexCoords.x, 0.001, 0.999); // avoid visual bugs at the bottom of the screen  
    reflectTexCoords.y = clamp(reflectTexCoords.y, -0.999, -0.001); // because of texCoords of <0, causing wraparound

    refractTexCoords += distortion;
    refractTexCoords = clamp(refractTexCoords, 0.0001, 0.9999);

    vec4 reflectColor = texture(texture_reflection, reflectTexCoords);
    vec4 refractColor = texture(texture_refraction, refractTexCoords);

    // Normal mapping: get normals from normalMap and put between -1,1
    vec4 normalColor = texture(texture_normal, distortionTexCoords);
    vec3 normalFromMap = vec3(normalColor.r * 2.0 - 1.0, normalColor.b, normalColor.g * 2.0 - 1.0); // remove normalColor.b*3.0 for original effect
    normalFromMap = normalize(normalFromMap);
    //vec3 combinedNormal = normalize(normal + normalFromMap); //--> !!! Required for water on a sphere. And on waves

    // Fresnel effect
    vec3 toCameraVectorNorm = normalize(cameraPos - fragPos_worldspace);
    float refractiveFactor = dot(toCameraVectorNorm, normal); // use vec3(0.0, 1.0, 0.0) as normal for original subtler effect
    refractiveFactor = pow(refractiveFactor, 2.0);

    // Normal mapping: calc specular light
    vec3 fromLightVector = fragPos_worldspace - light.position;
    //vec3 reflectedLight = reflect(normalize(fromLightVector), normal); // actual value of set light position
    vec3 reflectedLight = reflect(normalize(vec3(-0.2, -1.9f, -0.3f)), normal); // fixed value for directional light, much better
    float spec = pow(max(dot(reflectedLight, toCameraVectorNorm), 0.0), shineDamper);
    vec3 specular = light.color * spec * reflectivity * clamp(waterDepth/10.0, 0.0, 1.0);;

    vec4 retColor = mix(reflectColor, refractColor, refractiveFactor);
    retColor = mix(retColor, vec4(0.0, 0.3, 0.5, 1.0), 0.2) + vec4(specular, 0.0); // specular component add
    
    retColor.a = clamp(waterDepth/2.0, 0.0, 1.0); // From refraction Depth map -> gets rid of the ugly edges, but looks much worse from afar...
    return retColor;
}

void main() {
    fragColor = reflectRefract();
}