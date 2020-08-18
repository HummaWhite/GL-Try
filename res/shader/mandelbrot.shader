//$Vertex
#version 450 core
layout(location = 0) in vec2 pos;
out vec2 fragCoord;

uniform vec2 center;
uniform vec2 viewport;
uniform float zoom;

void main()
{
	vec2 ndc = pos * 2.0 - 1.0;
	fragCoord = ndc * viewport * zoom - center;
	gl_Position = vec4(ndc, 0.0, 1.0);
}

//$Fragment
#version 450 core
in vec2 fragCoord;
out vec4 FragColor;

uniform int maxIteration;
uniform float exitLimit;

float inMandelbrotSet(vec2 pos)
{
	const float LENGTH_CHECK = 10.0f;

	vec2 res = vec2(0.0);
	float count = 0.0;

	for (int i = 0; i < maxIteration; i++)
	{
		float x = res.x, y = res.y;
		res = vec2(x * x - y * y, 2 * x * y) + pos;
		count += 1.0;
		if (length(res) > exitLimit) break;
	}
	//return length(res) > LENGTH_CHECK ? 0.0 : 1.0;
	return count / float(maxIteration);
}

void main()
{
	FragColor = vec4(vec3(inMandelbrotSet(fragCoord)), 1.0);
}