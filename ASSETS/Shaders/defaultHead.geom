#version 330 core

struct DirLight {
    vec3 direction;

    vec4 ambient;
    vec4 diffuse;
    vec4 specular;

    float farPlane;

    mat4 lightSpaceMatrix;
};

#define MAX_POINT_LIGHTS 10
struct PointLight {
    vec3 position;

    vec4 ambient;
    vec4 diffuse;
    vec4 specular;

    float k0;
    float k1;
    float k2;

    float farPlane;
};

#define MAX_SPOT_LIGHTS 2
struct SpotLight {
    vec3 position;
    vec3 direction;

    float cutOff;
    float outerCutOff;

    vec4 ambient;
    vec4 diffuse;
    vec4 specular;

    float k0;
    float k1;
    float k2;

    float nearPlane;
    float farPlane;

    mat4 lightSpaceMatrix;
};

struct TangentLights {
    vec3 FragPos;
    vec3 Normal;

    vec3 viewPos;

    vec3 dirLightDirection;

    vec3 pointLightPositions[MAX_POINT_LIGHTS];

    vec3 spotLightPositions[MAX_SPOT_LIGHTS];
    vec3 spotLightDirections[MAX_SPOT_LIGHTS];
};

layout (std140) uniform Lights {
    DirLight dirLight;

    int noPointLights;
    PointLight pointLights[MAX_POINT_LIGHTS];

    int noSpotLights;
    SpotLight spotLights[MAX_SPOT_LIGHTS];
};