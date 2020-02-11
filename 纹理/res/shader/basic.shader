//$Vertex
#version 330 core
layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 aTexCoord;

uniform float additionY;
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
out vec2 texCoord;

void main()
{
    mat4 transform = proj * view * model;
    gl_Position = transform * vec4(pos, 1.0f);
    gl_Position.y += additionY;
    texCoord = aTexCoord;
}

//$Fragment
#version 330 core
out vec4 fragColor;
in vec2 texCoord;

uniform vec4 vertexColor;
uniform sampler2D u_Texture;

void main()
{
    fragColor = texture(u_Texture, texCoord) * vertexColor;
}