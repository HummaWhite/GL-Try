//$Vertex
#version 330 core
layout (location = 0) in vec2 pos;
layout (location = 1) in vec3 aColor;
uniform float additionY;
out vec3 additionColor;

void main()
{
   gl_Position = vec4(pos.x, pos.y + additionY, 0.0f, 1.0f);
   additionColor = aColor;
}

//$Fragment
#version 330 core
out vec4 fragColor;
in vec3 additionColor;

uniform vec4 vertexColor;

void main()
{
   fragColor = vertexColor;
   fragColor.z += additionColor.z;
   fragColor.z /= 2.0f;
}