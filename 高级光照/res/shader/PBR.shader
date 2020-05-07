//$Vertex
#version 450 core
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
#version 450 core
const float Pi = 3.1415926535897;
const int MAX_LIGHTS_DIR = 2;
const int MAX_LIGHTS_POINT = 8;
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

struct MaterialPBR
{
    vec3 albedo;
    float metallic;
    float roughness;
    float ao;
    sampler2D normalMap;
    sampler2D reflMap;
    float reflStrength;
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
    float cutOff;
    float outerCutOff;
    float strength;
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

uniform MaterialPBR material;
uniform int dirLightsCount;
uniform int pointLightsCount;
uniform DirLight dirLights[MAX_LIGHTS_DIR];
uniform PointLight pointLights[MAX_LIGHTS_POINT];
uniform vec3 viewPos;
uniform samplerCube depthMapPoint[MAX_LIGHTS_POINT];
uniform float farPlane;
uniform sampler2D ordTex;
uniform bool useTexture;
uniform bool useNormalMap;
uniform bool useReflMap;
uniform bool shadowOn;

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
    return (shadow / float(samples)) * float(shadowOn);
}

/*vec3 calcDirLight(int index, vec3 norm, vec3 viewDir)
{
    DirLight light = dirLights[index];
    vec3 lightDir = normalize(-light.dir);
    float diff = max(dot(norm, lightDir), 0.0f);
    float spec;
    if (useBlinnPhong)
    {
        vec3 halfwayDir = normalize(lightDir + viewDir);
        spec = pow(max(dot(halfwayDir, norm), 0.0), material.shininess * 4.0);
    }
    else
    {
        vec3 reflectDir = reflect(-lightDir, norm);
        spec = pow(max(dot(reflectDir, viewDir), 0.0), material.shininess);
    }
    vec3 ambient = light.color * material.ambient;
    vec3 diffuse = light.color * material.diffuse * diff;
    vec3 specular = light.color * (spec * material.specular);
    return ambient + diffuse + specular;
}*/

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
    float G   = GeometrySmith(N, V, L, material.roughness);

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
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    float shadow = shadowOn ? calcPointLightShadow(index, fragPos, light.pos) : 0.0;

    return Lo * (1.0 - shadow) * intensity;
}

vec2 sphereCoord(vec3 norm)
{
    vec3 ref = reflect(-normalize(viewPos - fs_in.fragPos), norm);
    float theta = atan(ref.y, ref.x);
    if (theta < 0.0) theta += Pi * 2;
    float phi = atan(length(ref.xy), ref.z);
    return vec2(theta / Pi / 2.0, phi / Pi);
}

void main()
{
    vec3 addNorm = texture(material.normalMap, fs_in.texCoord).rgb;
    addNorm = normalize(addNorm * 2.0 - 1.0);
    vec3 newNorm = useNormalMap ? normalize(fs_in.TBN * addNorm) : fs_in.normal;

    vec3 fragPos = fs_in.fragPos;
    vec3 V = normalize(viewPos - fragPos);

    vec3 result = vec3(0.03) * material.albedo * material.ao;

    /*for (int i = 0; i < dirLightsCount; i++)
        result += calcDirLight(i, newNorm, V);*/

    for (int i = 0; i < pointLightsCount; i++)
        result += calcPointLight(i, newNorm, fragPos, V);

    result = result / (result + vec3(1.0));

    result *= useTexture ? texture(ordTex, fs_in.texCoord).rgb : vec3(1.0f);
    vec3 reflColor = texture(material.reflMap, sphereCoord(newNorm)).rgb;
    float reflStrength = material.reflStrength;
    result = useReflMap ? reflStrength * reflColor + (1.0 - reflStrength) * result : result;
    fragColor = vec4(result, 1.0);
}