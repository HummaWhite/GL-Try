//$Vertex
#version 450 core
layout(location = 0) in vec2 aTexCoord;
out vec2 texCoord;

void main()
{
	texCoord = aTexCoord;
	vec2 transTex = aTexCoord * 2 - 1.0;
	gl_Position = vec4(transTex, 0.0, 1.0);
}

//$Fragment
#version 450 core
in vec2 texCoord;
out vec4 FragColor;

const vec3 BRIGHT_VEC = vec3(0.2126, 0.7152, 0.0722);

uniform sampler2D frameBuffer;
uniform float gamma;
uniform float exposure;

void main()
{
	vec2 texPos = texCoord;
	vec3 color = texture(frameBuffer, texPos).rgb;
	float brightness = dot(color, BRIGHT_VEC);
	float centerBrightness = dot(texture(frameBuffer, vec2(0.5, 0.5)).rgb, BRIGHT_VEC);
	vec3 mapped = vec3(1.0) - exp(-color * exposure);
	mapped = pow(mapped, vec3(1.0 / gamma));
	FragColor = vec4(mapped, 1.0);
}