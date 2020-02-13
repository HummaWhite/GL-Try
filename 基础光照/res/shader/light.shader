//$Vertex
#version 330 core
layout(location = 0) in vec3 pos;
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main()
{
    mat4 transform = proj * view * model;
    gl_Position = transform * vec4(pos * 0.3f, 1.0f);
}

//$Fragment
#version 330 core
out vec4 fragColor;
uniform vec3 lightColor;

void main()
{
    fragColor = vec4(lightColor, 1.0f);
}