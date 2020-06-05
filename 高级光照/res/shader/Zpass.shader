//$Vertex
#version 450 core
layout(location = 0) in vec3 pos;

out vec3 fragPos;

uniform mat4 model;
uniform mat4 VPmatrix;
uniform mat4 farplane;

void main()
{
    gl_Position = VPmatrix * model * vec4(pos, 1.0f);
    fragPos = gl_Position.xyz;
}

//$Fragment
#version 450 core
in vec3 fragPos;

out vec4 FragColor;

uniform float farPlane;

void main()
{
    float z = fragPos.z / farPlane;
    gl_FragDepth = z;
}