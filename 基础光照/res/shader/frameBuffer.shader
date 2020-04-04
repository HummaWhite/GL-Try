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
uniform sampler2D frameBuffer;
in vec2 texCoord;
out vec4 fragColor;

void main()
{
	fragColor = texture(frameBuffer, texCoord);
	fragColor = 1 - fragColor;
	fragColor.w = 1.0f;
}