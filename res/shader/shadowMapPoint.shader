//$Vertex
#version 450 core
layout(location = 0) in vec3 pos;

uniform mat4 model;

void main()
{
	gl_Position = model * vec4(pos, 1.0);
}

//$Fragment
#version 450 core
layout(location = 0) out vec4 FragColor;
in vec4 fragPos;
uniform vec3 lightPos;
uniform float farPlane;

void main()
{
	float lightDist = length(fragPos.xyz - lightPos);
	lightDist = lightDist / farPlane;
	gl_FragDepth = lightDist;
}

//$Geometry
#version 450 core
layout(triangles) in;
layout(triangle_strip, max_vertices = 18) out;

uniform mat4 lightVP[6];
out vec4 fragPos;

void main()
{
	for (int i = 0; i < 6; i++)
	{
		gl_Layer = i;
		for (int j = 0; j < 3; j++)
		{
			fragPos = gl_in[j].gl_Position;
			gl_Position = lightVP[i] * fragPos;
			EmitVertex();
		}
		EndPrimitive();
	}
}