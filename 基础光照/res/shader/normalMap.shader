//$Vertex
#version 430 core
layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform mat3 modelInv;
uniform float normDir;

out vec3 fragScrPos;

out VS_OUT
{
    vec2 texCoord;
    vec3 fragPos;
    vec3 normal;
    mat3 TBN;
} vs_out;

void main()
{
    mat4 transform = proj * view * model;
    gl_Position = transform * vec4(pos, 1.0f);
    vs_out.fragPos = vec3(model * vec4(pos, 1.0f));
    vs_out.texCoord = texCoord;
    vs_out.normal = normalize(modelInv * normal * normDir);
    vec3 T = normalize(vec3(model * vec4(tangent, 0.0)));
    vec3 B = normalize(vec3(model * vec4(bitangent, 0.0)));
    vec3 N = normalize(vec3(model * vec4(normal, 0.0)));
    vs_out.TBN = mat3(T, B, N) * normDir;
    fragScrPos = gl_Position.xyz;
}

//$Fragment
#version 430 core
const int MAX_LIGHTS_DIR = 2;
const int MAX_LIGHTS_POINT = 8;
const int MAX_LIGHTS_SPOT = 8;
const float AMBIENT_STRENGTH = 0.1f;
const float NEAR = 0.1;
const float FAR = 100.0;

const vec3 sampleOffsetDirections[20] = vec3[]
(
    vec3(1, 1, 1), vec3(1, -1, 1), vec3(-1, -1, 1), vec3(-1, 1, 1),
    vec3(1, 1, -1), vec3(1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
    vec3(1, 1, 0), vec3(1, -1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0),
    vec3(1, 0, 1), vec3(-1, 0, 1), vec3(1, 0, -1), vec3(-1, 0, -1),
    vec3(0, 1, 1), vec3(0, -1, 1), vec3(0, -1, -1), vec3(0, 1, -1)
);

struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
    sampler2D normalMap;
    sampler2D parallaxMap;
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

in VS_OUT
{
    vec2 texCoord;
    vec3 fragPos;
    vec3 normal;
    mat3 TBN;
} fs_in;

in vec3 fragScrPos;
out vec4 fragColor;

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
uniform bool useTexture;
uniform bool useNormalMap;
uniform float gamma;
uniform vec3 centerPos;

float calcPointLightShadow(int index, vec3 fragPos, vec3 lightPos)
{
    float shadow = 0.0;
    int samples = 20;
    float bias = 0.06;
    vec3 fragToLight = fragPos - lightPos;
    float currentDepth = length(fragToLight);
    float viewDis = length(viewPos - fragPos);
    float diskRad = (1.0 + (viewDis / farPlane)) / 25.0;
    for (int i = 0; i < samples; i++)
    {
        float closestDepth = texture(depthMapPoint[index], fragToLight + sampleOffsetDirections[i] * diskRad).r;
        closestDepth *= farPlane;
        if (currentDepth - bias > closestDepth) shadow += 1.0;
    }
    return shadow / float(samples);
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
    vec3 addNorm = texture(normMap, fs_in.texCoord).rgb;
    addNorm = normalize(addNorm * 2.0 - 1.0);
    vec3 newNorm = useNormalMap ? normalize(fs_in.TBN * addNorm) : fs_in.normal;
    vec3 fragPos = fs_in.fragPos;
    vec3 fragToView = normalize(viewPos - fragPos);
    for (int i = 0; i < dirLightsCount; i++)
        result += calcDirLight(i, newNorm, fragToView);
    for (int i = 0; i < pointLightsCount; i++)
        result += calcPointLight(i, newNorm, fragPos, fragToView);
    for (int i = 0; i < spotLightsCount; i++)
        result += calcSpotLight(i, newNorm, fragPos, fragToView);
    result *= useTexture ? texture(ordTex, fs_in.texCoord).rgb : vec3(1.0f);
    fragColor = vec4(result, 1.0);
}