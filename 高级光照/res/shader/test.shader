//$Vertex
#version 450 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in vec3 aTangent;

out vec3 fragPos;
out vec2 texCoord;
out vec3 normal;
out vec3 tangent;
out float fragDepth;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main()
{
    gl_Position = proj * view * model * vec4(aPos, 1.0);
    fragPos = vec3(model * vec4(aPos, 1.0));
    texCoord = aTexCoord;
    normal = aNormal;
    tangent = aTangent;
    fragDepth = gl_Position.z;
}

//$Fragment
#version 450 core

in vec3 fragPos;
in vec2 texCoord;
in vec3 normal;
in vec3 tangent;
in float fragDepth;

layout(location = 0) out vec4 PosColor;
layout(location = 1) out vec4 NormColor;
layout(location = 2) out vec4 DepthColor;
layout(location = 3) out vec4 TanColor;

void main()
{
    PosColor = vec4(fragPos, 1.0);
    NormColor = vec4(normal, 1.0);
    DepthColor = vec4(vec3(fragDepth / 10.0), 1.0);
    TanColor = vec4(tangent, 1.0);
}