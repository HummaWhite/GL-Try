#include "Shape.h"

#include <queue>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

float* createCone(int faces, float radius, float height)
{
	std::queue<float> Q;
	int vertexCount = faces * 3 * 8;
	float* a = new float[vertexCount];
	float alpha = glm::radians(360.0f / (float)faces);
	for (int i = 0; i < faces; i++)
	{
		float x1 = radius * cos(i * alpha), y1 = radius * sin(i * alpha);
		float x2 = radius * cos((i + 1) * alpha), y2 = radius * sin((i + 1) * alpha);
		glm::vec3 va(x1, y1, -height);
		glm::vec3 vb(x2, y2, -height);
		glm::vec3 norm = glm::normalize(glm::cross(va, vb));
		Q.push(x1), Q.push(y1), Q.push(0), Q.push(0), Q.push(0), Q.push(norm.x), Q.push(norm.y), Q.push(norm.z);
		Q.push(x2), Q.push(y2), Q.push(0), Q.push(0), Q.push(0), Q.push(norm.x), Q.push(norm.y), Q.push(norm.z);
		Q.push(0), Q.push(0), Q.push(height), Q.push(0), Q.push(0), Q.push(norm.x), Q.push(norm.y), Q.push(norm.z);
	}
	for (int i = 0; i < vertexCount; i++)
	{
		a[i] = Q.front();
		Q.pop();
	}
	return a;
}

float* createSphere(int columns, int rows, float radius, float Atheta, float Arho)
{
	int vertexCount = rows * columns * 6 * 8;
	float* a = new float[vertexCount];
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
	for (int i = 0; i < vertexCount; i++)
	{
		a[i] = Q.front();
		Q.pop();
	}
	return a;
}

float* addShapeWithTangents(const float* ordShape, int trianglesCount)
{
	if (ordShape == nullptr) return nullptr;
	int dataCount = trianglesCount * 3 * 14;
	float* a = new float[dataCount];
	std::queue<float> Q;
	for (int i = 0; i < trianglesCount; i++)
	{
		glm::vec3 p1(ordShape[i * 24 + 0], ordShape[i * 24 + 1], ordShape[i * 24 + 2]);
		glm::vec3 p2(ordShape[i * 24 + 8], ordShape[i * 24 + 9], ordShape[i * 24 + 10]);
		glm::vec3 p3(ordShape[i * 24 + 16], ordShape[i * 24 + 17], ordShape[i * 24 + 18]);
		glm::vec2 uv1(ordShape[i * 24 + 3], ordShape[i * 24 + 4]);
		glm::vec2 uv2(ordShape[i * 24 + 11], ordShape[i * 24 + 12]);
		glm::vec2 uv3(ordShape[i * 24 + 19], ordShape[i * 24 + 20]);

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
				Q.push(ordShape[i * 24 + j * 8 + k]);
			Q.push(tangent.x), Q.push(tangent.y), Q.push(tangent.z);
			Q.push(btangent.x), Q.push(btangent.y), Q.push(btangent.z);
		}
	}
	for (int i = 0; i < dataCount; i++)
	{
		a[i] = Q.front();
		Q.pop();
	}
	return a;
}