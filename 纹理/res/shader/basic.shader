//$Vertex
#version 330 core
layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec3 aColor;
uniform float additionY;
out vec3 additionColor;
out vec2 texCoord;

void main()
{
   gl_Position = vec4(pos.x, pos.y + additionY, 0.0f, 1.0f);
   additionColor = aColor;
   texCoord = aTexCoord;
}

//$Fragment
#version 330 core
out vec4 fragColor;
in vec3 additionColor;
in vec2 texCoord;

uniform vec4 vertexColor;
uniform sampler2D u_Texture;

void main()
{
   fragColor = texture(u_Texture, texCoord) * vertexColor;
   fragColor.z += additionColor.z;
   fragColor.z /= 2.0f;
}