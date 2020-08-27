//$Vertex
#version 450 core
layout(location = 0) in vec2 pos;

out vec2 texCoord;

void main()
{
	vec2 ndc = pos * 2.0 - 1.0;
	gl_Position = vec4(ndc, 0.0, 1.0);
	texCoord = pos;
}

//$Fragment
#version 450 core
in vec2 texCoord;

uniform sampler2D ssaoMap;

void main()
{
	float result = 0.0;
	
	float texelSize = 1.0 / textureSize(ssaoMap, 0).y;
	float radius = 0.5;

	int size = 5;

	for (int i = -size; i <= size; i++)
	{
		for (int j = -size; j <= size; j++)
		{
			vec2 sampleCoord = texCoord + vec2(i, j) * radius * texelSize;
			result += 1.0 - texture(ssaoMap, sampleCoord).r;
		}
	}

	gl_FragDepth = 1.0 - result / (4.0 * size * size - 4.0 * size + 1.0);
}