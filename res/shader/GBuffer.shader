//$Vertex
#version 450 core
layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 tangent;

uniform mat4 model;
uniform mat4 VPmatrix;
uniform mat3 modelInv;

out VSOut
{
    vec2 texCoord;
    vec3 fragPos;
    vec3 normal;
    mat3 TBN;
    vec3 fragScrPos;
} vsOut;

void main()
{
    gl_Position = VPmatrix * model * vec4(pos, 1.0f);
    vsOut.fragPos = vec3(model * vec4(pos, 1.0f));
    vsOut.texCoord = texCoord;
    vsOut.normal = normalize(modelInv * normal);
    vec3 bitangent = normalize(cross(normal, tangent));
    vec3 T = normalize(vec3(model * vec4(tangent, 0.0)));
    vec3 B = normalize(vec3(model * vec4(bitangent, 0.0)));
    vec3 N = normalize(vec3(model * vec4(normal, 0.0)));
    vsOut.TBN = mat3(T, B, N);
    vsOut.fragScrPos = gl_Position.xyz;
}

//$Fragment
#version 450 core

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

in VSOut
{
    vec2 texCoord;
    vec3 fragPos;
    vec3 normal;
    mat3 TBN;
    vec3 fragScrPos;
} fsIn;

layout(location = 0) out vec4 Position;
layout(location = 1) out vec4 Normal;
layout(location = 2) out vec4 Albedo;
layout(location = 3) out vec4 MetRouAo;

uniform MaterialPBR material;
uniform sampler2D ordTex;
uniform bool useTexture;
uniform bool useNormalMap;
uniform bool useReflMap;
uniform bool forceFlatNormals;

uniform bool enableZCull;
uniform sampler2D ssZMap;
uniform float ssZMapDivisor;
uniform float posDivisor;
uniform vec2 viewport;

void preZCull()
{
    const float cullEPS = 0.01;
    vec2 scrCoord = gl_FragCoord.xy / viewport;
    float minDepth = texture(ssZMap, scrCoord).r * ssZMapDivisor;
    if (fsIn.fragScrPos.z > minDepth + cullEPS) discard;
}

void main()
{
    if (enableZCull) preZCull();

    vec3 fragPos = fsIn.fragPos;
    vec2 texCoord = fsIn.texCoord;
    vec3 addNorm = texture(material.normalMap, texCoord).rgb;
    addNorm = normalize(addNorm * 2.0 - 1.0);
    vec3 newNorm = useNormalMap ? normalize(fsIn.TBN * addNorm) : fsIn.normal;
    newNorm = forceFlatNormals ? normalize(cross(dFdx(fragPos), dFdy(fragPos))) : newNorm;

    Position = vec4(((fsIn.fragPos / posDivisor) + 1.0) / 2.0, 1.0);
    Normal = vec4((newNorm + 1.0) / 2.0, 1.0);
    Albedo = vec4(material.albedo, 1.0);
    MetRouAo = vec4(material.metallic, material.roughness, material.ao, 1.0);
}