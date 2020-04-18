//$Vertex
#version 430 core

void main()
{
}

//$Fragment
#version 430 core
out vec4 fragColor;
/*in vec3 fragPos;
in vec2 texCoord;
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
	return C[n][i] * (i * pow(u, i - 1) * pow(1 - u, n - i) - (n - i) * pow(u, i) * pow(1 - u, n - i - 1));
}

vec2 sphereTo2D(vec3 sph)
{
	float theta = atan(sph.y, sph.x);
	if (theta < 0.0) theta += Pi * 2;
	float phi = atan(length(sph.xy), sph.z);
	return vec2(theta / Pi / 2.0, phi / Pi);
}*/

void main()
{
	/*vec3 tan = vec3(0.0);
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
	fragColor = texture(tex, coord);*/
	fragColor = vec4(1.0);
}

//$Geometry
#version 430 core
layout(points) in;
layout(triangle_strip, max_vertices = 120) out;

uniform vec3 points[16];
uniform int N;
uniform int M;
uniform int secU;
uniform int secV;
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

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
	return C[n][i] * (i * pow(u, i - 1) * pow(1 - u, n - i) - (n - i) * pow(u, i) * pow(1 - u, n - i - 1));
}

void main()
{
	float du = 1.0 / float(secU);
	float dv = 1.0 / float(secV);
	for (int i = 0; i < secU; i++)
	{
		for (int j = 0; j < secV; j++)
		{
			float u = i * du, v = j * dv;
			float up = (i + 1) * du, vp = (j + 1) * dv;
			vec3 P1 = vec3(0.0), P2 = vec3(0.0), P3 = vec3(0.0), P4 = vec3(0.0);
			for (int s = 0; s <= N; s++)
			{
				for (int t = 0; t <= M; t++)
				{
					vec3 Pst = points[s * (M + 1) + t];
					P1 += B(N, s, u) * B(M, t, v) * Pst;
					P2 += B(N, s, up) * B(M, t, v) * Pst;
					P3 += B(N, s, up) * B(M, t, vp) * Pst;
					P4 += B(N, s, u) * B(M, t, vp) * Pst;
				}
			}
			gl_Position = proj * view * model * vec4(P1, 1.0); EmitVertex();
			gl_Position = proj * view * model * vec4(P2, 1.0); EmitVertex();
			gl_Position = proj * view * model * vec4(P3, 1.0); EmitVertex();
			EndPrimitive();
			/*gl_Position = vec4(P1, 1.0); EmitVertex();
			gl_Position = vec4(P3, 1.0); EmitVertex();
			gl_Position = vec4(P4, 1.0); EmitVertex();
			EndPrimitive();*/
		}
	}
}