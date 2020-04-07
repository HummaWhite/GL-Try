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
uniform float gamma;
uniform float exposure;

void main()
{
	vec2 texPos = texCoord;
	vec3 color = texture(frameBuffer, texPos).rgb;
	vec3 mapped = vec3(1.0) - exp(-color * exposure);
	mapped = pow(mapped, vec3(1.0 / gamma));
	fragColor = vec4(mapped, 1.0);
}
//texPos = floor(texPos * 128) / 128;
//fragColor = floor(fragColor * 8) / 8;
//fragColor.w = 1.0;
//fragColor = vec4(vec3(fragColor.r + fragColor.g + fragColor.b) / 3, 1.0);