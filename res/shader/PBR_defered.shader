//$Vertex
#version 450 core
layout(location = 0) in vec2 aTexCoord;
out vec2 texCoord;

void main()
{
    texCoord = aTexCoord;
    vec2 transTex = aTexCoord * 2 - 1.0;
    gl_Position = vec4(transTex, 0.0, 1.0);
}

//$Fragment
#version 450 core
const float Pi = 3.1415926535897;
const int MAX_LIGHTS_DIR = 2;
const int MAX_LIGHTS_POINT = 8;
const int BLOCKER_SEARCH_NUM_SAMPLES = 20;
const int PCF_NUM_SAMPLES = 20;

const vec3 sampleDisk[20] = vec3[]
(
    vec3(1, 1, 1), vec3(1, -1, 1), vec3(-1, -1, 1), vec3(-1, 1, 1),
    vec3(1, 1, -1), vec3(1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
    vec3(1, 1, 0), vec3(1, -1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0),
    vec3(1, 0, 1), vec3(-1, 0, 1), vec3(1, 0, -1), vec3(-1, 0, -1),
    vec3(0, 1, 1), vec3(0, -1, 1), vec3(0, -1, -1), vec3(0, 1, -1)
);

vec3 DEBUG_COLOR;

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
    float cutoff;
    float outerCutoff;
    float strength;
    float size;
};

in vec2 texCoord;
out vec4 FragColor;

uniform float nearPlane;
uniform int dirLightsCount;
uniform int pointLightsCount;
uniform DirLight dirLights[MAX_LIGHTS_DIR];
uniform PointLight pointLights[MAX_LIGHTS_POINT];
uniform samplerCube shadowMapPoint[MAX_LIGHTS_POINT];
uniform float shadowNearPlane;
uniform float shadowFarPlane;
uniform float posDivisor;
uniform bool shadowOn;

uniform sampler2D posTex;
uniform sampler2D normTex;
uniform sampler2D albedoTex;
uniform sampler2D metRouAoTex;

uniform vec3 viewPos;

struct MaterialPBR
{
    vec3 albedo;
    float metallic;
    float roughness;
    float ao;
};

MaterialPBR material;

//////////////////////////////////<<<PCSS>>>//////////////////////////////////

float shadowBiasPoint(float texelSize, vec3 fragToLight, vec3 N)
{
    float cosTheta = dot(normalize(fragToLight), N);
    float tanTheta = abs(sqrt(1 - cosTheta * cosTheta) / cosTheta);
    return texelSize * tanTheta * length(fragToLight) * 20.0 + 0.01;
}

float penumbraSize(float zReceiver, float zBlocker)
{
    return (zReceiver - zBlocker) / zBlocker;
}

void findBlocker(
    out float   avgBlockerDepth,
    out float   numBlockers,
    samplerCube shadowCubeMap,
    vec3        lightToFrag,
    vec3        N,
    float       zReceiver,
    float       lightSizeUV)
{
    float searchWidth = lightSizeUV * (zReceiver - nearPlane) / zReceiver * 0.01;

    float blockerSum = 0;
    numBlockers = 0;
    vec3 uv = normalize(lightToFrag);
    float texelSize = 2.0 * shadowNearPlane / float(textureSize(shadowCubeMap, 0).x);
    float bias = shadowBiasPoint(texelSize, -lightToFrag, N);

    for (int i = 0; i < BLOCKER_SEARCH_NUM_SAMPLES; i++)
    {
        float shadowMapDepth = texture(shadowCubeMap, uv + sampleDisk[i] * searchWidth).r;
        shadowMapDepth *= shadowFarPlane;
        if (shadowMapDepth + bias < zReceiver)
        {
            blockerSum += shadowMapDepth;
            numBlockers += 1.0;
        }
    }
    avgBlockerDepth = blockerSum / (numBlockers);
}

float PCFcubeMap(samplerCube shadowCubeMap, vec3 lightToFrag, vec3 N, float zReceiver, float diskRad)
{
    float sum = 0;
    vec3 uv = normalize(lightToFrag);
    
    float texelSize = 2.0 * shadowNearPlane / float(textureSize(shadowCubeMap, 0).x);
    float bias = shadowBiasPoint(texelSize, -lightToFrag, N);

    for (int i = 0; i < PCF_NUM_SAMPLES; i++)
    {
        float shadowMapDepth = texture(shadowCubeMap, uv + sampleDisk[i] * diskRad).r;
        shadowMapDepth *= shadowFarPlane;
        if (zReceiver > shadowMapDepth + bias) sum += 1.0;
    }
    return sum / float(PCF_NUM_SAMPLES);
}

float PCSScubeMap(samplerCube shadowCubeMap, vec3 lightToFrag, vec3 N, float lightSizeUV)
{
    float avgBlockerDepth = 0;
    float numBlockers = 0;
    float zReceiver = length(lightToFrag);
    findBlocker(avgBlockerDepth, numBlockers, shadowCubeMap, lightToFrag, N, zReceiver, lightSizeUV);

    if (numBlockers < 1.0) return 0.0;

    float filterRad = 0.001 + penumbraSize(zReceiver, avgBlockerDepth) * 0.001;
    return PCFcubeMap(shadowCubeMap, lightToFrag, N, zReceiver, filterRad);
}

//////////////////////////////////<<<PBR>>>//////////////////////////////////

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = Pi * denom * denom;

    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 calcPointLight(int index, vec3 N, vec3 fragPos, vec3 V)
{
    PointLight light = pointLights[index];

    vec3 L = normalize(light.pos - fragPos);
    vec3 H = normalize(V + L);

    vec3 F0 = mix(vec3(0.04), material.albedo, material.metallic);

    vec3  F = fresnelSchlick(max(dot(V, H), 0.0), F0);
    float NDF = DistributionGGX(N, H, material.roughness);
    float G = GeometrySmith(N, V, L, material.roughness);

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - material.metallic;

    vec3 nominator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;
    vec3 specular = nominator / denominator;

    float dist = length(light.pos - fragPos);
    float attenuation = 1.0 / (
        light.attenuation.x +
        light.attenuation.y * dist +
        light.attenuation.z * dist * dist);
    vec3 radiance = light.color * light.strength * attenuation;

    float NdotL = max(dot(N, L), 0.0);
    vec3 Lo = (kD * material.albedo / Pi + specular) * radiance * NdotL;

    float theta = dot(L, normalize(-light.dir));
    float epsilon = light.cutoff - light.outerCutoff;
    float intensity = clamp((theta - light.outerCutoff) / epsilon, 0.0, 1.0);
    float shadow = float(shadowOn) * PCSScubeMap(shadowMapPoint[index], fragPos - light.pos, N, light.size);

    return Lo * (1.0 - shadow) * intensity;
}

vec2 sphereCoord(vec3 V, vec3 norm)
{
    vec3 ref = reflect(-V, norm);
    float theta = atan(ref.y, ref.x);
    if (theta < 0.0) theta += Pi * 2;
    float phi = atan(length(ref.xy), ref.z);
    return vec2(theta / Pi / 2.0, phi / Pi);
}

void main()
{
    vec3 fragPos = ((texture(posTex, texCoord).rgb * 2.0) - 1.0) * posDivisor;
    vec3 norm = (texture(normTex, texCoord).rgb * 2.0) - 1.0;
    material.albedo = texture(albedoTex, texCoord).rgb;
    material.metallic = texture(metRouAoTex, texCoord).r;
    material.roughness = texture(metRouAoTex, texCoord).g;
    material.ao = texture(metRouAoTex, texCoord).b;

    vec3 V = normalize(viewPos - fragPos);

    vec3 result = vec3(0.03) * material.albedo * material.ao;

    for (int i = 0; i < pointLightsCount; i++)
        result += calcPointLight(i, norm, fragPos, V);

    result = result / (result + vec3(1.0));

    FragColor = vec4(result, 1.0);
}