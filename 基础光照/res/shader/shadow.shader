//$Vertex
#version 430 core
layout(location = 0) in vec3 pos;

uniform mat4 model;

void main()
{
	gl_Position = model * vec4(pos, 1.0);
}

//$Fragment
#version 430 core

in vec4 fragPos;
uniform vec3 lightPos;
uniform float farPlane;
out vec4 fragColor;

void main()
{
	// get distance between fragment and light source
	float lightDistance = length(fragPos.xyz - lightPos);

	// map to [0;1] range by dividing by far_plane
	lightDistance = lightDistance / farPlane;

	// write this as modified depth
	gl_FragDepth = lightDistance;
	fragColor = vec4(vec3(lightDistance), 1.0);
}

//$Geometry
#version 430 core
layout(triangles) in;
layout(triangle_strip, max_vertices = 18) out;

uniform mat4 shadowMatrices[6];
out vec4 fragPos;

void main()
{
	for (int i = 0; i < 6; i++)
	{
		gl_Layer = i;
		for (int j = 0; j < 3; j++)
		{
			fragPos = gl_in[j].gl_Position;
			gl_Position = shadowMatrices[i] * fragPos;
			EmitVertex();
		}
		EndPrimitive();
	}
}