//$Vertex
#version 430 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;
out vec2 texCoord;
out vec3 fragPos;

//uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main()
{
	texCoord = aTexCoord;
	mat4 model = mat4(1.0);
	vec4 pos = proj * view * model * vec4(aPos, 1.0);
	fragPos = vec3(model * vec4(aPos, 1.0));
	gl_Position = pos.xyww;
	texCoord = aTexCoord;
}

//$Fragment
#version 430 core
out vec4 fragColor;
in vec2 texCoord;
in vec3 fragPos;

uniform sampler2D sky;

void main()
{
	fragColor = texture(sky, texCoord);
}