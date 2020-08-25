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

out float ssao;

const int SAMPLES_COUNT = 64;

uniform sampler2D depMetRouMap;
uniform sampler2D normalMap;
uniform sampler2D noiseMap;

uniform mat4 proj;
uniform mat4 projInv;

uniform vec2 viewport;
uniform vec3 samples[SAMPLES_COUNT];
uniform float radius;

uniform float nearPlane;
uniform float farPlane;

float linearDepth(float depth)
{
	float z = depth * 2.0 - 1.0;
	return (2.0 * nearPlane * farPlane) / (farPlane + nearPlane - z * (farPlane - nearPlane));
}

void main()
{
	vec2 noiseScale = viewport / 4.0;

	float depth = texture(depMetRouMap, texCoord).r;
	if (depth < 0.01) discard;

	vec3 normal = texture(normalMap, texCoord).rgb;
	vec3 noise = normalize(texture(noiseMap, noiseScale * texCoord).xyz);

	vec3 ssPos = vec3(texCoord, depth);
	vec4 ndcPos = vec4(ssPos * 2.0 - 1.0, 1.0);
	vec4 cvvPos = projInv * ndcPos;
	vec3 fragPos = cvvPos.xyz / cvvPos.w;

	vec3 tangent = normalize(noise - normal * dot(noise, normal));
	vec3 bitangent = cross(normal, tangent);
	mat3 TBN = mat3(tangent, bitangent, normal);

	float occlusion = 0.0;

	for (int i = 0; i < SAMPLES_COUNT; i++)
	{
		vec3 sampleVec = TBN * samples[i];
		sampleVec = fragPos + sampleVec * radius;

		vec4 offset = vec4(sampleVec, 1.0);
		offset = proj * offset;

		vec3 ssSample = offset.xyz / offset.w;
		ssSample = (ssSample + 1.0) / 2.0;

		float sampleDepth = linearDepth(texture(depMetRouMap, ssSample.xy).r);

		const float bias = 0.025;

		float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
		rangeCheck = 1.0;
		occlusion += ((sampleDepth > (linearDepth(ssSample.z) + bias)) ? 1.0 : 0.0) * rangeCheck;
	}

	gl_FragDepth = 1.0 - occlusion / SAMPLES_COUNT;
}