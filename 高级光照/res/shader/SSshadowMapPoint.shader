//$Vertex
#version 450 core
layout(location = 0) in vec2 pos;
out vec2 scrCoord;

void main()
{
	gl_Position = vec4(pos, 0.0, 1.0);
	scrCoord = pos;
}

//$Fragment
#version 450 core
const int MAX_LIGHTS_POINT = 8;
in vec2 scrCoord;

uniform sampler2D ZMap;
uniform samplerCube shadowMapPoint[MAX_LIGHTS_POINT];
uniform int pointLightCount;
uniform mat4 VPinv;

void main()
{
	vec3 ssPos = vec3(scrCoord, texture(ZMap, scrCoord).r);
	vec3 fragPos = vec3(VPinv * vec4(ssPos, 1.0));

	for (int i = 0; i < pointLightCount; i++)
	{
	}
}