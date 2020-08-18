//$Vertex
#version 450 core
layout(location = 0) in vec3 pos;

out vec3 fragScrPos;

uniform mat4 model;
uniform mat4 VPmatrix;

void main()
{
    gl_Position = VPmatrix * model * vec4(pos, 1.0f);
    fragScrPos = gl_Position.xyz;
}

//$Fragment
#version 450 core
in vec3 fragScrPos;

uniform float farPlane;

void main()
{
    float z = fragScrPos.z / farPlane;
    gl_FragDepth = z;
}