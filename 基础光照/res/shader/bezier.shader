//$Vertex
#version 430 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec3 aNormal;
out vec3 fragPos;
out vec2 texCoord;
out vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main()
{
	gl_Position = proj * view * model * vec4(aPos, 1.0);
	fragPos = vec3(model * vec4(aPos, 1.0));
	texCoord = aTexCoord;
	normal = aNormal;
}

//$Fragment
#version 430 core
in vec3 fragPos;
in vec2 texCoord;
in vec3 normal;
out vec4 fragColor;

uniform vec3 viewPos;
uniform sampler2D tex;
uniform vec3 points[16];
uniform int N;
uniform int M;

const float Pi = 3.1415926535897;

const float C[5][5] =
{
	{ 1, 0, 0, 0, 0 },
	{ 1, 1, 0, 0, 0 },
	{ 1, 2, 1, 0, 0 },
	{ 1, 3, 3, 1, 0 },
	{ 1, 4, 6, 4, 1 }
};

float B(int n, int i, float u)
{
	return C[n][i] * pow(u, i) * pow(1 - u, n - i);
}

float dB(int n, int i, float u)
{
	return C[n][i] * (i == 0 ? 0 : i * pow(u, i - 1) * pow(1 - u, n - i))
		- C[n][i] * (i == n ? 0 : (n - i) * pow(u, i) * pow(1 - u, n - i - 1));
}

vec2 sphereTo2D(vec3 sph)
{
	float theta = atan(sph.y, sph.x);
	if (theta < 0.0) theta += Pi * 2;
	float phi = atan(length(sph.xy), sph.z);
	return vec2(theta / Pi / 2.0, phi / Pi);
}

void main()
{
	vec3 tan = vec3(0.0);
	vec3 btn = vec3(0.0);
	float u = texCoord.x, v = texCoord.y;
	for (int i = 0; i <= N; i++)
	{
		for (int j = 0; j <= M; j++)
		{
			vec3 Pij = points[i * (M + 1) + j];
			tan += dB(N, i, u) * B(M, j, v) * Pij;
			btn += B(N, i, u) * dB(M, j, v) * Pij;
		}
	}
	vec3 norm = normalize(cross(tan, btn));
	vec3 ref = reflect(-normalize(viewPos - fragPos), norm);
	vec2 coord = sphereTo2D(ref);
	fragColor = texture(tex, coord);
}