//$Vertex
#version 430 core
layout(location = 0) in vec3 pos;
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main()
{
    mat4 transform = proj * view * model;
    gl_Position = transform * vec4(pos, 1.0f);
}

//$Fragment
#version 430 core
uniform vec3 lightColor;

void main()
{
    gl_FragColor = vec4(lightColor, 1.0f);
}