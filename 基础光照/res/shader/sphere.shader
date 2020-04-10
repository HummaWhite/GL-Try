//$Vertex
#version 430 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec3 aNormal;
out vec3 fragPos;
out vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main()
{
	gl_Position = proj * view * model * vec4(aPos, 1.0);
	fragPos = vec3(model * vec4(aPos, 1.0));
	normal = aNormal;
}

//$Fragment
#version 430 core
in vec3 fragPos;
in vec3 normal;
out vec4 fragColor;

uniform vec3 viewPos;
uniform vec3 center;
uniform sampler2D tex;

const float Pi = 3.1415926535897;

void main()
{
	vec3 norm = normalize(fragPos - center);
	vec3 ref = reflect(-normalize(viewPos - fragPos), norm);
	float theta = atan(ref.y, ref.x);
	if (theta < 0.0) theta += Pi * 2;
	float phi = atan(length(ref.xy), ref.z);
	vec2 coord = vec2(theta / Pi / 2.0, phi / Pi);
	fragColor = texture(tex, coord);
}