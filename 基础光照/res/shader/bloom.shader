//$Vertex
#version 430 core
layout(location = 0) in vec2 aTexCoord;
out vec2 texCoord;

void main()
{
	texCoord = aTexCoord;
	vec2 transTex = aTexCoord * 2 - 1.0;
	gl_Position = vec4(transTex, 0.0, 1.0);
}

//$Fragment
#version 430 core
in vec2 texCoord;
out vec4 fragColor;

uniform sampler2D frameBuffer;

void main()
{
	vec2 texPos = texCoord;
	vec3 color = texture(frameBuffer, texPos).rgb;
	float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));
	//if (brightness > 1.0) fragColor = vec4(color, 1.0);
	//else fragColor = vec4(0.0);
	fragColor = texture(frameBuffer, texPos);
}