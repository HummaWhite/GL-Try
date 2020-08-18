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

void main()
{
    vec3 irradiance = vec3(0.0);
    vec3 N = planeToSphere(uv2);

    vec3 U = vec3(0.0, 0.0, 1.0);
    vec3 R = normalize(cross(N, U));
    U = cross(R, N);
    
    float delta = 0.02;
    float samplesCount = 0.0;

    for (float t = 0.0; t <= 2.0 * Pi; t += delta)
    {
        for (float p = 0.0; p <= 0.5 * Pi; p += delta)
        {
            vec3 T = vec3(cos(t) * sin(p), sin(t) * sin(p), cos(p));
            vec3 V = normalize(mat3(U, R, N) * T);

            irradiance += texture(envMap, sphereToPlane(V)).rgb * cos(p) * sin(p);
            samplesCount += 1.0;
        }
    }

    irradiance *= Pi / samplesCount;
    
    FragColor = vec4(irradiance, 1.0);
}