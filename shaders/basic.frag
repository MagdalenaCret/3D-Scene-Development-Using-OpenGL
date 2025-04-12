#version 410 core

in vec3 fPosition;
in vec4 fPosEye;
in vec3 fNormal;
in vec2 fTexCoords;
in vec4 fragPosLightSpace;

out vec4 fColor;

// Uniforms
uniform int fogCond;
uniform bool isDay; // Toggle for day or night

// Matrices
uniform mat4 model;
uniform mat4 view;
uniform mat3 normalMatrix;

// Lighting
uniform vec3 lightDir;
uniform vec3 lightColor;

// Textures
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D shadowMap;

// Components
vec3 ambient;
float ambientStrength = 0.2f;
vec3 diffuse;
vec3 specular;
float specularStrength = 0.5f;
float shininess = 32.0f;
float shadow = 0.0f;

void computeDirLight() {
    vec4 fPosEye = view * model * vec4(fPosition, 1.0f);
    vec3 normalEye = normalize(normalMatrix * fNormal);
    vec3 lightDirN = normalize((view * vec4(lightDir, 0.0f)).xyz);
    vec3 viewDir = normalize(-fPosEye.xyz);

    // Adjust ambient light based on day or night
    vec3 dayAmbient = vec3(1.0f, 1.0f, 1.0f);
    vec3 nightAmbient = vec3(0.1f, 0.1f, 0.2f);
    ambient = ambientStrength * mix(nightAmbient, dayAmbient, isDay ? 1.0f : 0.0f);

    // Adjust diffuse light based on day or night
    vec3 dayDiffuse = lightColor;
    vec3 nightDiffuse = vec3(0.2f, 0.2f, 0.5f);
    diffuse = max(dot(normalEye, lightDirN), 0.0f) * mix(nightDiffuse, dayDiffuse, isDay ? 1.0f : 0.0f);

    // Adjust specular light (dimmed at night)
    vec3 reflectDir = reflect(-lightDirN, normalEye);
    specular = specularStrength * pow(max(dot(viewDir, reflectDir), 0.0f), shininess) *
               mix(vec3(0.1f), lightColor, isDay ? 1.0f : 0.0f);
}

float computeShadow() {
    vec3 normalizedCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    normalizedCoords = normalizedCoords * 0.5 + 0.5;

    if (normalizedCoords.z > 1.0f) {
        return 0.0f;
    }

    float closestDepth = texture(shadowMap, normalizedCoords.xy).r;
    float currentDepth = normalizedCoords.z;
    float bias = 0.005f;

    return currentDepth - bias > closestDepth ? 1.0f : 0.0f;
}

float computeFog() {
    float fogDensity = isDay ? 0.05f : 0.1f; // Thicker fog at night
    float fragmentDistance = length(fPosEye);
    float fogFactor = exp(-pow(fragmentDistance * fogDensity, 2));

    return clamp(fogFactor, 0.0f, 1.0f);
}

void main() {
    computeDirLight();
    shadow = computeShadow();

    float fogFactor = computeFog();
    vec4 fogColor = isDay ? vec4(0.5f, 0.5f, 0.5f, 1.0f) : vec4(0.1f, 0.1f, 0.2f, 1.0f); // Lighter fog during day, darker at night

    ambient *= texture(diffuseTexture, fTexCoords).rgb;
    diffuse *= texture(diffuseTexture, fTexCoords).rgb;
    specular *= texture(specularTexture, fTexCoords).rgb;

    vec3 color = min((ambient + (1.0f - shadow) * diffuse) + (1.0f - shadow) * specular, 1.0f);

    if (fogCond == 0)
        fColor = fogColor * (1 - fogFactor) + vec4(color, 0.0f) * fogFactor;
    else
        fColor = vec4(color, 1.0f);
}
