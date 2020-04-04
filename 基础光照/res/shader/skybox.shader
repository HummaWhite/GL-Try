//$Vertex
#version 430 core
layout(location = 0) in vec3 aPos;
out vec3 texCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main()
{
	texCoord = aPos;
	vec4 pos = proj * view * model * vec4(aPos, 1.0);
	gl_Position = pos.xyww;
}

//$Fragment
#version 430 core
out vec4 fragColor;
in vec3 texCoord;

uniform samplerCube skybox;

void main()
{
	fragColor = texture(skybox, texCoord);
}