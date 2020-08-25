//$Vertex
#version 450 core
layout(location = 0) in vec3 pos;

uniform mat4 model;
uniform mat4 VPmatrix;

void main()
{
    gl_Position = VPmatrix * model * vec4(pos, 1.0f);
}

//$Fragment
#version 450 core

void main()
{
}