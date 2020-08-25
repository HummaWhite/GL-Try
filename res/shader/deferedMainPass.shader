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
const int MAX_LIGHTS_POINT = 4;
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
uniform float farPlane;
uniform int dirLightsCount;
uniform int pointLightsCount;
uniform DirLight dirLights[MAX_LIGHTS_DIR];
uniform PointLight pointLights[MAX_LIGHTS_POINT];
uniform vec3 viewPos;
uniform samplerCube shadowMapPoint[MAX_LIGHTS_POINT];
uniform float shadowNearPlane;
uniform float shadowFarPlane;
uniform bool shadowOn;
uniform bool enableSSAO;

uniform int maxPrefilterMipLevels;
uniform sampler2D irradianceMap;
uniform sampler2D prefilterMap;
uniform sampler2D lutMap;

uniform sampler2D depMetRouMap;
uniform sampler2D normalMap;
uniform sampler2D albedoMap;
uniform sampler2D aoMap;
uniform mat4 VPinv;

vec3 fragPos;
vec3 albedo;
float metallic;
float roughness;
float ao;

vec2 sphereToPlane(vec3 uv)
{
    float theta = atan(uv.y, uv.x);
    if (theta < 0.0) theta += Pi * 2;
    float phi = atan(length(uv.xy), uv.z);
    return vec2(theta / Pi / 2.0, phi / Pi);
}

vec3 planeToSphere(vec2 uv)
{
    float theta = uv.x * Pi * 2;
    float phi = uv.y * Pi;
    return vec3(cos(theta) * sin(phi), sin(theta) * sin(phi), cos(phi));
}

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

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

float distributionGGX(vec3 N, vec3 H, float roughness)
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

float geometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float geometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = geometrySchlickGGX(NdotV, roughness);
    float ggx1 = geometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 calcPointLight(int index, vec3 N, vec3 V)
{
    PointLight light = pointLights[index];

    vec3 L = normalize(light.pos - fragPos);
    vec3 H = normalize(V + L);

    vec3 F0 = mix(vec3(0.04), albedo, metallic);

    vec3  F = fresnelSchlick(max(dot(V, H), 0.0), F0);
    float D = distributionGGX(N, H, roughness);
    float G = geometrySmith(N, V, L, roughness);

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;

    vec3 FDG = D * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;
    vec3 specular = FDG / denominator;

    float dist = length(light.pos - fragPos);
    float attenuation = 1.0 / (
        light.attenuation.x +
        light.attenuation.y * dist +
        light.attenuation.z * dist * dist);
    vec3 Li = light.color * light.strength * attenuation;

    float NdotL = max(dot(N, L), 0.0);
    vec3 Lo = (kD * albedo / Pi + specular) * Li * NdotL;

    float theta = dot(L, normalize(-light.dir));
    float epsilon = light.cutoff - light.outerCutoff;
    float intensity = clamp((theta - light.outerCutoff) / epsilon, 0.0, 1.0);
    float shadow = float(shadowOn) * PCSScubeMap(shadowMapPoint[index], fragPos - light.pos, N, light.size);

    return Lo * (1.0 - shadow) * intensity;
}

vec3 IBLColor(vec3 N, vec3 V)
{
    vec3 res = vec3(0.0);

    vec3 F0 = mix(vec3(0.04), albedo, metallic);
    vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);

    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;

    vec3 irradiance = texture(irradianceMap, sphereToPlane(N)).rgb;
    vec3 diffuse = irradiance * albedo;

    vec3 R = reflect(-V, N);
    vec3 prefilteredColor = textureLod(prefilterMap, sphereToPlane(R), roughness * float(maxPrefilterMipLevels - 1)).rgb;
    vec2 BRDF = texture(lutMap, vec2(max(dot(N, V), 0.0), roughness)).rg;

    vec3 specular = prefilteredColor * (F * BRDF.x + BRDF.y);
    vec3 ambient = (kD * diffuse + specular) * ao;

    // 我还说天空盒从哪里画出来的，原来是这里啊！！！！
    // 采用的是延迟渲染，gBuffer（albedoMap、normalMap、depMetRouMap）没有物体的地方的值全为0
    // 这就是说，这些没有物体绘制的像素的albedo = vec3(0.0)、N = vec3(0.0)、metallic = roughness = 0.0
    // 所以在上面的计算中，diffuse = vec3(0.0)，而R的值，根据glsl的定义，为-V - 2.0 * dot(N, -V) * N;
    // 即-V，而又有-V = normalize(fragPos - viewPos)。深度为0，相当于每个像素都在在近裁平面上，
    // 通过Proj、View的逆变换后得到的fragPos与viewPos相减就是观察的方向
    // prefilterColor取了prefilterMap对应方向上的0级mipmap，而这个mipmap的内容即为未经过重要性采样的环境贴图
    // 即天空盒贴图本身
    // BRDF的值取了lutMap的(0, 0)像素，即(0.0, 1.0, 0.0)，故specular算出来等于prefilterColor
    // 由于ao为1.0，最后的结果显然等于prefilterColor，即天空盒对应方向的颜色
    // 前面计算每个光源的影响与此类似，发现每个的结果都为0，所以最终得到的片元颜色就只有天空盒的颜色！
    // 这都能hack！！！！

    return ambient;
}

void main()
{
    vec3 depMetRou = texture(depMetRouMap, texCoord).rgb;

    float fragDepth = depMetRou.r;
    if (fragDepth < 0.01) discard;

    vec3 ssPos = vec3(texCoord, fragDepth);
    vec4 ndcPos = vec4(ssPos * 2.0 - 1.0, 1.0);
    vec4 cvvPos = VPinv * ndcPos;

    fragPos = cvvPos.xyz / cvvPos.w;
    albedo = texture(albedoMap, texCoord).rgb;
    ao = enableSSAO ? texture(aoMap, texCoord).r : 1.0;
    metallic = depMetRou.g;
    roughness = depMetRou.b;

    vec3 N = texture(normalMap, texCoord).xyz;
    vec3 V = normalize(viewPos - fragPos);

    vec3 result = IBLColor(N, V);

    for (int i = 0; i < pointLightsCount; i++)
    {
        result += calcPointLight(i, N, V);
    }

    result = result / (result + vec3(1.0));

    FragColor = vec4(result, 1.0);
    gl_FragDepth = fragDepth;
}