//$Vertex
#version 450 core
layout(location = 0) in vec2 scrCoord;
out vec2 uv2;

void main()
{
    vec2 ndc = scrCoord * 2.0 - 1.0;
    gl_Position = vec4(ndc, 0.0, 1.0);
    uv2 = scrCoord;
}

//$Fragment
#version 450 core
in vec2 uv2;

out vec4 FragColor;

uniform sampler2D envMap;
uniform float roughness;

const float Pi = 3.14159265358979323846;

vec2 sphereToPlane(vec3 uv)
{
    float theta = atan(uv.y, uv.x);
    if (theta < 0.0) theta += Pi * 2;
    float phi = atan(length(uv.xy), uv.z);
    if (phi < 0.0) phi += Pi * 2;
    return vec2(theta / Pi / 2.0, phi / Pi);
}

vec3 planeToSphere(vec2 uv)
{
    float theta = uv.x * Pi * 2;
    float phi = uv.y * Pi;
    return vec3(cos(theta) * sin(phi), sin(theta) * sin(phi), cos(phi));
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

float radicalInverseVDC(uint bits)
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10;
}

vec2 hammersley(uint i, uint n)
{
    return vec2(float(i) / float(n), radicalInverseVDC(i));
}

vec3 importanceSampleGGX(vec2 xi, vec3 N, float roughness)
{
    float r4 = pow(roughness, 4.0);

    float phi = 2.0 * Pi * xi.x;
    float cosTheta = sqrt((1.0 - xi.y) / (1.0 + (r4 - 1.0) * xi.y));
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

    vec3 H = vec3(cos(phi) * sinTheta, sin(phi) * sinTheta, cosTheta);

    vec3 U = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 T = normalize(cross(U, N));
    vec3 B = cross(N, T);

    return normalize(mat3(T, B, N) * H);
}

void main()
{
    vec3 result = vec3(0.0);

    vec3 N = planeToSphere(uv2);

    vec3 R = N;
    vec3 V = R;

    const uint MAX_SAMPLES = 1024;
    float weightSum = 0.0;

    for (uint i = 0; i < MAX_SAMPLES; i++)
    {
        vec2 xi = hammersley(i, MAX_SAMPLES);

        vec3 H = importanceSampleGGX(xi, N, roughness);
        vec3 L = normalize(2.0 * dot(V, H) * H - V);

        float NdotL = max(dot(N, L), 0.0);
        float NdotH = max(dot(N, H), 0.0);
        float HdotV = max(dot(H, V), 0.0);

        float D = distributionGGX(N, H, roughness);
        float pdf = D * NdotH / (4.0 * HdotV) + 0.0001;

        vec2 resolution = textureSize(envMap, 0).xy;
        float saTexel = 4.0 * Pi / (resolution.x * resolution.y);
        float saSample = 1.0 / (float(MAX_SAMPLES) * pdf + 0.0001);

        float mipLevel = (roughness == 0.0) ? 0.0 : 0.5 * log2(saSample / saTexel);

        result += textureLod(envMap, sphereToPlane(L), mipLevel).rgb * NdotL;
        weightSum += NdotL;
    }

    result /= weightSum;

    FragColor = vec4(result, 1.0);
}