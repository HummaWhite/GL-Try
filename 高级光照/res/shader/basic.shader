//$Vertex
#version 430 core
layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform mat3 modelInv;
uniform float normDir;

out vec2 texCoord;
out vec3 fragPos;
out vec3 normal;

void main()
{
    mat4 transform = proj * view * model;
    gl_Position = transform * vec4(pos, 1.0f);
    fragPos = vec3(model * vec4(pos, 1.0f));
    texCoord = aTexCoord;
    normal = normalize(modelInv * aNormal * normDir);
}

//$Fragment
#version 430 core
const int MAX_LIGHTS_DIR = 2;
const int MAX_LIGHTS_POINT = 8;
const int MAX_LIGHTS_SPOT = 8;
const float AMBIENT_STRENGTH = 0.1f;
const float NEAR = 0.1;
const float FAR = 100.0;

struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
    sampler2D diffuseTex;
    sampler2D specularTex;
    sampler2D normalMap;
};

struct DirLight
{
    vec3 dir;
    vec3 color;
};

struct PointLight
{
    vec3 pos;
    vec3 dir;
    vec3 color;
    vec3 attenuation;
};

struct SpotLight
{
    vec3 pos;
    vec3 dir;
    vec3 color;
    vec3 attenuation;
    float cutOff;
    float outerCutOff;
};

out vec4 fragColor;
in vec3 fragPos;
in vec3 normal;
in vec2 texCoord;

uniform Material material;
uniform int dirLightsCount;
uniform int pointLightsCount;
uniform int spotLightsCount;
uniform DirLight dirLights[MAX_LIGHTS_DIR];
uniform PointLight pointLights[MAX_LIGHTS_POINT];
uniform SpotLight spotLights[MAX_LIGHTS_SPOT];
uniform vec3 viewPos;
uniform vec4 vertexColor;
uniform samplerCube depthMapPoint[MAX_LIGHTS_POINT];
uniform float farPlane;
uniform sampler2D ordTex;
uniform sampler2D normMap;

float calcPointLightShadow(int index, vec3 fragPos, vec3 lightPos)
{
    vec3 fragToLight = fragPos - lightPos;
    float closestDepth = texture(depthMapPoint[index], fragToLight).r;
    closestDepth *= farPlane;
    float currentDepth = length(fragToLight);
    float bias = 0.052;
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
    return shadow;
}

vec3 calcDirLight(int index, vec3 norm, vec3 viewDir)
{
    DirLight light = dirLights[index];
    vec3 lightDir = normalize(-light.dir);
    float diff = max(dot(norm, lightDir), 0.0f);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 ambient = light.color * material.ambient;
    vec3 diffuse = light.color * material.diffuse * diff;
    vec3 specular = light.color * (spec * material.specular);
    return ambient + diffuse + specular;
}

vec3 calcPointLight(int index, vec3 norm, vec3 fragPos, vec3 viewDir)
{
    PointLight light = pointLights[index];
    vec3 lightDir = normalize(light.pos - fragPos);
    float diff = max(dot(norm, lightDir), 0.0f);
    vec3 reflectDir = reflect(-lightDir, norm);
    float dist = length(light.pos - fragPos);
    float attenuation = 1.0 / (
        light.attenuation.x +
        light.attenuation.y * dist +
        light.attenuation.z * dist * dist);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 ambient = light.color * material.ambient;
    vec3 diffuse = light.color * material.diffuse * diff;
    vec3 specular = light.color * (spec * material.specular);
    float shadow = calcPointLightShadow(index, fragPos, light.pos);
    return (ambient + (1.0f - shadow) * (diffuse + specular)) * attenuation;
}

vec3 calcSpotLight(int index, vec3 norm, vec3 fragPos, vec3 viewDir)
{
    SpotLight light = spotLights[index];
    vec3 lightDir = normalize(light.pos - fragPos);
    float diff = max(dot(norm, lightDir), 0.0f);
    vec3 reflectDir = reflect(-lightDir, norm);
    float dist = length(light.pos - fragPos);
    float attenuation = 1.0 / (
        light.attenuation.x +
        light.attenuation.y * dist +
        light.attenuation.z * dist * dist);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 ambient = light.color * material.ambient;
    vec3 diffuse = light.color * material.diffuse * diff;
    vec3 specular = light.color * (spec * material.specular);
    float theta = dot(lightDir, normalize(-light.dir));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    return (ambient + diffuse + specular) * attenuation * intensity;
}

float linearDepth(float depth)
{
    float z = depth * 2.0 - 1.0;
    return (2.0 * NEAR * FAR) / (FAR + NEAR - z * (FAR - NEAR));
}

void main()
{
    vec3 result = vec3(0, 0, 0);
    vec3 newNorm = normal;
    vec3 addNorm = texture(normMap, texCoord).rgb;
    addNorm = normalize(addNorm * 2.0 - 1.0);
    newNorm = normal;
    for (int i = 0; i < dirLightsCount; i++)
        result += calcDirLight(i, newNorm, normalize(viewPos - fragPos));
    for (int i = 0; i < pointLightsCount; i++)
        result += calcPointLight(i, newNorm, fragPos, normalize(viewPos - fragPos));
    for (int i = 0; i < spotLightsCount; i++)
        result += calcSpotLight(i, newNorm, fragPos, normalize(viewPos - fragPos));
    result *= vec3(texture(ordTex, texCoord));
    fragColor = vec4(result, 1.0);
}