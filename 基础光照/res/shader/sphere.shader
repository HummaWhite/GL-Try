//$Vertex
#version 430 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;
out vec3 fragPos;
out vec2 texCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main()
{
	gl_Position = proj * view * model * vec4(aPos, 1.0);
	fragPos = vec3(model * vec4(aPos, 1.0));
	texCoord = aTexCoord;
}

//$Fragment
#version 430 core
in vec3 fragPos;
in vec2 texCoord;
out vec4 fragColor;

uniform vec3 center;
uniform sampler2D tex;

const float Pi = 3.1415926535897;

void main()
{
	vec3 norm = normalize(fragPos - center);
	fragColor = texture(tex, texCoord);
	fragColor.w = 0.5f;
}