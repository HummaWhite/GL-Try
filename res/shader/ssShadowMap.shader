//$Vertex
#version 450 core
layout(location = 0) in vec2 pos;
out vec2 scrCoord;

void main()
{
	vec2 transPos = pos * 2.0 - 1.0;
	gl_Position = vec4(transPos, 0.0, 1.0);
	scrCoord = pos;
}

//$Fragment
#version 450 core
const int MAX_LIGHTS_POINT = 8;

in vec2 scrCoord;
out vec4 FragColor;

uniform sampler2D ssZMap;
uniform float ssZMapNear;
uniform float ssZMapFar;
uniform samplerCube shadowMapPoint[MAX_LIGHTS_POINT];
uniform vec3 lightPos[MAX_LIGHTS_COUNT];
uniform int pointLightCount;
uniform mat4 VPinv;

float shadow(samplerCube shadowMap, vec3 fragPos, vec3 lightPos)
{
	vec3 L = lightPos - fragPos;
	float shadowMapDepth = texture(shadowMap, -L);
	float fragDepth = length(L);

}

void main()
{
	vec3 ndcPos = vec3(scrCoord, texture(ssZMap, scrCoord).r);
	vec4 ndc = vec4(ndcPos * 2.0 - 1.0, 1.0);
	vec4 fragPos = VPinv * ndc;
	fragPos.xyz /= fragPos.w;
	FragColor = vec4(fragPos.xyz, 1.0);
}