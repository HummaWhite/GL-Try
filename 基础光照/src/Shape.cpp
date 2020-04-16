#include "Shape.h"

#include <queue>
#include <fstream>
#include <sstream>
#include <cstring>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

float C[5][5] =
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

Shape::Shape(int vertexCount, int type):
	m_VertexCount(vertexCount), m_Type(type), m_WithTangents(false)
{
	m_Layout.add<GL_FLOAT>(3);
	m_Layout.add<GL_FLOAT>(2);
	m_Layout.add<GL_FLOAT>(3);
}

void Shape::addTangents()
{
	if (m_Buffer == nullptr)
	{
		std::cout << "No data yet" << std::endl;
		return;
	}
	if (m_WithTangents)
	{
		std::cout << "Already added tangents" << std::endl;
		return;
	}
	int trianglesCount = m_VertexCount / 3;
	float* tmp = new float[m_VertexCount * 14];
	std::queue<float> Q;
	for (int i = 0; i < trianglesCount; i++)
	{
		glm::vec3 p1(m_Buffer[i * 24 + 0], m_Buffer[i * 24 + 1], m_Buffer[i * 24 + 2]);
		glm::vec3 p2(m_Buffer[i * 24 + 8], m_Buffer[i * 24 + 9], m_Buffer[i * 24 + 10]);
		glm::vec3 p3(m_Buffer[i * 24 + 16], m_Buffer[i * 24 + 17], m_Buffer[i * 24 + 18]);
		glm::vec2 uv1(m_Buffer[i * 24 + 3], m_Buffer[i * 24 + 4]);
		glm::vec2 uv2(m_Buffer[i * 24 + 11], m_Buffer[i * 24 + 12]);
		glm::vec2 uv3(m_Buffer[i * 24 + 19], m_Buffer[i * 24 + 20]);

		glm::vec3 dp1 = p2 - p1;
		glm::vec3 dp2 = p3 - p1;
		glm::vec2 duv1 = uv2 - uv1;
		glm::vec2 duv2 = uv3 - uv1;

		float f = 1.0f / (duv1.x * duv2.y - duv2.x * duv1.y);

		float tanX = f * (duv2.y * dp1.x - duv1.y * dp2.x);
		float tanY = f * (duv2.y * dp1.y - duv1.y * dp2.y);
		float tanZ = f * (duv2.y * dp1.z - duv1.y * dp2.z);
		float btanX = f * (-duv2.x * dp1.x + duv1.x * dp2.x);
		float btanY = f * (-duv2.x * dp1.y + duv1.x * dp2.y);
		float btanZ = f * (-duv2.x * dp1.z + duv1.x * dp2.z);

		glm::vec3 tangent = glm::normalize(glm::vec3(tanX, tanY, tanZ));
		glm::vec3 btangent = glm::normalize(glm::vec3(btanX, btanY, btanZ));

		for (int j = 0; j < 3; j++)
		{
			for (int k = 0; k < 8; k++)
				Q.push(m_Buffer[i * 24 + j * 8 + k]);
			Q.push(tangent.x), Q.push(tangent.y), Q.push(tangent.z);
			Q.push(btangent.x), Q.push(btangent.y), Q.push(btangent.z);
		}
	}
	for (int i = 0; i < m_VertexCount * 14; i++)
	{
		tmp[i] = Q.front();
		Q.pop();
	}
	delete[]m_Buffer;
	m_Buffer = tmp;
	m_WithTangents = true;
	m_Layout.add<GL_FLOAT>(3);
	m_Layout.add<GL_FLOAT>(3);
}

void Shape::setBuffer(float* buffer)
{
	m_Buffer = buffer;
}

Cube::Cube() :
	Shape(36, CUBE)
{
	float* buffer = new float[288];
	memcpy(buffer, CUBE_VERTICES, 288 * sizeof(float));
	setBuffer(buffer);
}

Square::Square() :
	Shape(6, SQUARE)
{
	float* buffer = new float[48];
	memcpy(buffer, SQUARE_VERTICES, 48 * sizeof(float));
	setBuffer(buffer);
}

Cone::Cone(int faces, float radius, float height) :
	Shape(faces * 3, CONE)
{
	std::queue<float> Q;
	int vertexCount = faces * 3;
	float* buffer = new float[vertexCount * 8];
	float alpha = glm::radians(360.0f / (float)faces);
	for (int i = 0; i < faces; i++)
	{
		float x1 = radius * cos(i * alpha), y1 = radius * sin(i * alpha);
		float x2 = radius * cos((i + 1) * alpha), y2 = radius * sin((i + 1) * alpha);
		glm::vec3 va(x1, y1, -height);
		glm::vec3 vb(x2, y2, -height);
		glm::vec3 norm = glm::normalize(glm::cross(va, vb));
		float sqx1 = 1.0f / faces * i;
		float sqx2 = 1.0f / faces * (i + 1);
		Q.push(x1), Q.push(y1), Q.push(0), Q.push(sqx1), Q.push(1.0), Q.push(norm.x), Q.push(norm.y), Q.push(norm.z);
		Q.push(x2), Q.push(y2), Q.push(0), Q.push(sqx2), Q.push(1.0), Q.push(norm.x), Q.push(norm.y), Q.push(norm.z);
		Q.push(0), Q.push(0), Q.push(height), Q.push(sqx1), Q.push(0.0), Q.push(norm.x), Q.push(norm.y), Q.push(norm.z);
	}
	for (int i = 0; i < vertexCount * 8; i++)
	{
		buffer[i] = Q.front();
		Q.pop();
	}
	setBuffer(buffer);
}

Sphere::Sphere(int columns, int rows, float radius, float Atheta, float Arho) :
	Shape(rows * columns * 6, SPHERE)
{
	int vertexCount = rows * columns * 6;
	float* buffer = new float[vertexCount * 8];
	float theta = glm::radians(Atheta / (float)columns);
	float rho = glm::radians(Arho / (float)rows);
	std::queue<float> Q;
	for (int i = 0; i < columns; i++)
	{
		for (int j = 0; j < rows; j++)
		{
			float z1 = radius * cos(j * rho);
			float z2 = radius * cos((j + 1) * rho);
			float x1 = radius * sin(j * rho) * cos(i * theta);
			float x2 = radius * sin(j * rho) * cos((i + 1) * theta);
			float x3 = radius * sin((j + 1) * rho) * cos(i * theta);
			float x4 = radius * sin((j + 1) * rho) * cos((i + 1) * theta);
			float y1 = radius * sin(j * rho) * sin(i * theta);
			float y2 = radius * sin(j * rho) * sin((i + 1) * theta);
			float y3 = radius * sin((j + 1) * rho) * sin(i * theta);
			float y4 = radius * sin((j + 1) * rho) * sin((i + 1) * theta);
			glm::vec3 va(x3 - x2, y3 - y2, z2 - z1);
			glm::vec3 vb(x4 - x1, y4 - y1, z2 - z1);
			glm::vec3 norm = glm::normalize(glm::cross(va, vb));
			//glm::vec3 norm1 = glm::normalize(glm::vec3(x1, y1, z1));
			//glm::vec3 norm2 = glm::normalize(glm::vec3(x2, y2, z1));
			//glm::vec3 norm3 = glm::normalize(glm::vec3(x3, y3, z2));
			//glm::vec3 norm4 = glm::normalize(glm::vec3(x4, y4, z2));
			float sqx1 = 1.0f / columns * i;
			float sqx2 = 1.0f / columns * (i + 1);
			float sqy1 = 1.0f / rows * j;
			float sqy2 = 1.0f / rows * (j + 1);
			Q.push(x1), Q.push(y1), Q.push(z1), Q.push(sqx1), Q.push(sqy1), Q.push(norm.x), Q.push(norm.y), Q.push(norm.z);
			Q.push(x2), Q.push(y2), Q.push(z1), Q.push(sqx2), Q.push(sqy1), Q.push(norm.x), Q.push(norm.y), Q.push(norm.z);
			Q.push(x3), Q.push(y3), Q.push(z2), Q.push(sqx1), Q.push(sqy2), Q.push(norm.x), Q.push(norm.y), Q.push(norm.z);
			Q.push(x4), Q.push(y4), Q.push(z2), Q.push(sqx2), Q.push(sqy2), Q.push(norm.x), Q.push(norm.y), Q.push(norm.z);
			Q.push(x3), Q.push(y3), Q.push(z2), Q.push(sqx1), Q.push(sqy2), Q.push(norm.x), Q.push(norm.y), Q.push(norm.z);
			Q.push(x2), Q.push(y2), Q.push(z1), Q.push(sqx2), Q.push(sqy1), Q.push(norm.x), Q.push(norm.y), Q.push(norm.z);
		}
	}
	for (int i = 0; i < vertexCount * 8; i++)
	{
		buffer[i] = Q.front();
		Q.pop();
	}
	setBuffer(buffer);
}

Torus::Torus(int columns, int rows, float majorRadius, float minorRadius, float Atheta, float Btheta) :
	Shape(columns* rows * 6, TORUS)
{
	int vertexCount = columns * rows * 6;
	float* buffer = new float[vertexCount * 8];
	float dAtheta = glm::radians(Atheta / (float)columns);
	float dBtheta = glm::radians(Btheta / (float)rows);
	std::queue<float> Q;
	for (int i = 0; i < columns; i++)
	{
		for (int j = 0; j < rows; j++)
		{
			float z1 = minorRadius * sin(j * dBtheta);
			float z2 = minorRadius * sin((j + 1) * dBtheta);
			float x1 = (majorRadius - minorRadius * cos(j * dBtheta)) * cos(i * dAtheta);
			float x2 = (majorRadius - minorRadius * cos(j * dBtheta)) * cos((i + 1) * dAtheta);
			float x3 = (majorRadius - minorRadius * cos((j + 1) * dBtheta)) * cos(i * dAtheta);
			float x4 = (majorRadius - minorRadius * cos((j + 1) * dBtheta)) * cos((i + 1) * dAtheta);
			float y1 = (majorRadius - minorRadius * cos(j * dBtheta)) * sin(i * dAtheta);
			float y2 = (majorRadius - minorRadius * cos(j * dBtheta)) * sin((i + 1) * dAtheta);
			float y3 = (majorRadius - minorRadius * cos((j + 1) * dBtheta)) * sin(i * dAtheta);
			float y4 = (majorRadius - minorRadius * cos((j + 1) * dBtheta)) * sin((i + 1) * dAtheta);
			glm::vec3 va(x3 - x2, y3 - y2, z2 - z1);
			glm::vec3 vb(x4 - x1, y4 - y1, z2 - z1);
			glm::vec3 norm = glm::normalize(glm::cross(va, vb));
			float sqx1 = 1.0f / columns * i;
			float sqx2 = 1.0f / columns * (i + 1);
			float sqy1 = 1.0f / rows * j;
			float sqy2 = 1.0f / rows * (j + 1);
			Q.push(x1), Q.push(y1), Q.push(z1), Q.push(sqx1), Q.push(sqy1), Q.push(norm.x), Q.push(norm.y), Q.push(norm.z);
			Q.push(x2), Q.push(y2), Q.push(z1), Q.push(sqx2), Q.push(sqy1), Q.push(norm.x), Q.push(norm.y), Q.push(norm.z);
			Q.push(x3), Q.push(y3), Q.push(z2), Q.push(sqx1), Q.push(sqy2), Q.push(norm.x), Q.push(norm.y), Q.push(norm.z);
			Q.push(x4), Q.push(y4), Q.push(z2), Q.push(sqx2), Q.push(sqy2), Q.push(norm.x), Q.push(norm.y), Q.push(norm.z);
			Q.push(x3), Q.push(y3), Q.push(z2), Q.push(sqx1), Q.push(sqy2), Q.push(norm.x), Q.push(norm.y), Q.push(norm.z);
			Q.push(x2), Q.push(y2), Q.push(z1), Q.push(sqx2), Q.push(sqy1), Q.push(norm.x), Q.push(norm.y), Q.push(norm.z);
		}
	}
	for (int i = 0; i < vertexCount * 8; i++)
	{
		buffer[i] = Q.front();
		Q.pop();
	}
	setBuffer(buffer);
}

Bezier::Bezier(int _n, int _m, int _secU, int _secV, const std::vector<glm::vec3>& points) :
	Shape(_secU * _secV * 6, BEZIER), n(_n), m(_m), secU(_secU), secV(_secV)
{
	if ((n + 1) * (m + 1) != points.size())
	{
		std::cout << "Bezier: vector size not equal to n * m" << std::endl;
		return;
	}
	int vertexCount = secU * secV * 6;
	float* buffer = new float[vertexCount * 8];
	std::queue<float> Q;

	float du = 1.0 / (float)secU;
	float dv = 1.0 / (float)secV;

	for (int i = 0; i < secU; i++)
	{
		for (int j = 0; j < secV; j++)
		{
			float u = i * du, v = j * dv;
			float up = (i + 1) * du, vp = (j + 1) * dv;
			glm::vec3 P1(0.0), P2(0.0), P3(0.0), P4(0.0);
			for (int s = 0; s <= n; s++)
			{
				for (int t = 0; t <= m; t++)
				{
					glm::vec3 Pst = points[s * (m + 1) + t];
					P1 += B(n, s, u)  * B(m, t, v)  * Pst;
					P2 += B(n, s, up) * B(m, t, v)  * Pst;
					P3 += B(n, s, up) * B(m, t, vp) * Pst;
					P4 += B(n, s, u)  * B(m, t, vp) * Pst;
					//std::cout << P1.x << " " << P1.y << " " << P1.z << std::endl;
				}
			}
			glm::vec3 norm123 = glm::normalize(glm::cross(P2 - P1, P3 - P1));
			glm::vec3 norm134 = glm::normalize(glm::cross(P3 - P1, P4 - P1));
			Q.push(P1.x), Q.push(P1.y), Q.push(P1.z), Q.push(u),  Q.push(v),  Q.push(norm123.x), Q.push(norm123.y), Q.push(norm123.z);
			Q.push(P2.x), Q.push(P2.y), Q.push(P2.z), Q.push(up), Q.push(v),  Q.push(norm123.x), Q.push(norm123.y), Q.push(norm123.z);
			Q.push(P3.x), Q.push(P3.y), Q.push(P3.z), Q.push(up), Q.push(vp), Q.push(norm123.x), Q.push(norm123.y), Q.push(norm123.z);
			Q.push(P1.x), Q.push(P1.y), Q.push(P1.z), Q.push(u),  Q.push(v),  Q.push(norm134.x), Q.push(norm134.y), Q.push(norm134.z);
			Q.push(P3.x), Q.push(P3.y), Q.push(P3.z), Q.push(up), Q.push(vp), Q.push(norm134.x), Q.push(norm134.y), Q.push(norm134.z);
			Q.push(P4.x), Q.push(P4.y), Q.push(P4.z), Q.push(u),  Q.push(vp), Q.push(norm134.x), Q.push(norm134.y), Q.push(norm134.z);
		}
	}
	for (int i = 0; i < vertexCount * 8; i++)
	{
		buffer[i] = Q.front();
		Q.pop();
	}
	setBuffer(buffer);
}
