//$Vertex
#version 330 core
layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform mat3 modelInv;

out vec2 texCoord;
out vec3 fragPos;
out vec3 normal;

void main()
{
    mat4 transform = proj * view * model;
    gl_Position = transform * vec4(pos, 1.0f);
    fragPos = vec3(model * vec4(pos, 1.0));
    normal = modelInv * aNormal;
    texCoord = aTexCoord;
}

//$Fragment
#version 330 core
const int MAX_LIGHTS_DIR = 2;
const int MAX_LIGHTS_POINT = 5;
const int MAX_LIGHTS_SPOT = 5;
const float AMBIENT_STRENGTH = 0.1f;

struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
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

uniform Material material;
uniform int dirLightsCount;
uniform int pointLightsCount;
uniform int spotLightsCount;
uniform DirLight dirLights[MAX_LIGHTS_DIR];
uniform PointLight pointLights[MAX_LIGHTS_POINT];
uniform SpotLight spotLights[MAX_LIGHTS_SPOT];
uniform vec3 viewPos;
uniform vec4 vertexColor;

vec3 calcDirLight(DirLight light, vec3 norm, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.dir);
    float diff = max(dot(norm, lightDir), 0.0f);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 ambient = light.color * material.ambient;
    vec3 diffuse = light.color * material.diffuse * diff;
    vec3 specular = light.color * (spec * material.specular);
    return ambient + diffuse + specular;
}

vec3 calcPointLight(PointLight light, vec3 norm, vec3 fragPos, vec3 viewDir)
{
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
    return (ambient + diffuse + specular) * attenuation;
}

vec3 calcSpotLight(SpotLight light, vec3 norm, vec3 fragPos, vec3 viewDir)
{
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
    return (ambient + diffuse + specular) * intensity * attenuation;
}

void main()
{
    vec3 result = vec3(0.0f, 0.0f, 0.0f);
    for (int i = 0; i < dirLightsCount; i++)
        result += calcDirLight(dirLights[i], normal, normalize(viewPos - fragPos));
    for (int i = 0; i < pointLightsCount; i++)
        result += calcPointLight(pointLights[i], normal, fragPos, normalize(viewPos - fragPos));
    for (int i = 0; i < spotLightsCount; i++)
        result += calcSpotLight(spotLights[i], normal, fragPos, normalize(viewPos - fragPos));
    fragColor = vec4(result, 1.0f);
}