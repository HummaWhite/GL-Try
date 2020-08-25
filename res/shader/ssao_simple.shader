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

uniform sampler2D depthMap;
uniform sampler2D noiseMap;
uniform mat4 proj;
uniform vec3 samples[64];
uniform float radius;
uniform float nearPlane;
uniform float farPlane;

const vec2 noiseScale = vec2(1920.0 / 4.0, 1080.0 / 4.0);

float linearDepth(float depth)
{
	float z = depth * 2.0 - 1.0;
	return (2.0 * nearPlane * farPlane) / (farPlane + nearPlane - z * (farPlane - nearPlane));
}

void main()
{
	float depth = texture(depthMap, texCoord).r;
	//vec3 noise = texture(noiseMap, noiseScale * texCoord).xyz;

	vec3 ssPos = vec3(texCoord, depth);
	vec4 ndcPos = vec4(ssPos * 2.0 - 1.0, 1.0);
	vec4 cvvPos = inverse(proj) * ndcPos;
	vec3 fragPos = cvvPos.xyz / cvvPos.w;

	float occlusion = 0.0;
	int samplesCount = 64;

	for (int i = 0; i < samplesCount; i++)
	{
		vec3 sp = fragPos + samples[i] * radius;

		vec4 offset = vec4(sp, 1.0);
		offset = proj * offset;

		vec3 ssSample = offset.xyz / offset.w;
		ssSample = (ssSample + 1.0) / 2.0;
		float sampleDepth = linearDepth(texture(depthMap, ssSample.xy).r);

		const float bias = 0.001;

		float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
		rangeCheck = 1.0;
		occlusion += ((linearDepth(ssSample.z) > sampleDepth + bias) ? 1.0 : 0.0) * rangeCheck;
	}

	occlusion = 1.0 - occlusion / float(samplesCount);
	
	gl_FragDepth = occlusion;
}