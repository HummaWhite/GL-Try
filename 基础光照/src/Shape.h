#pragma once
#pragma GCC optimize(3, "Ofast", "inline")

#include <iostream>
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

float* createSphere(int columns, int rows, float radius, float Atheta = 360.0f, float Arho = 180.0f)
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
			Q.push(x1), Q.push(y1), Q.push(z1), Q.push(0), Q.push(0), Q.push(norm.x), Q.push(norm.y), Q.push(norm.z);
			Q.push(x2), Q.push(y2), Q.push(z1), Q.push(0), Q.push(0), Q.push(norm.x), Q.push(norm.y), Q.push(norm.z);
			Q.push(x3), Q.push(y3), Q.push(z2), Q.push(0), Q.push(0), Q.push(norm.x), Q.push(norm.y), Q.push(norm.z);
			Q.push(x4), Q.push(y4), Q.push(z2), Q.push(0), Q.push(0), Q.push(norm.x), Q.push(norm.y), Q.push(norm.z);
			Q.push(x3), Q.push(y3), Q.push(z2), Q.push(0), Q.push(0), Q.push(norm.x), Q.push(norm.y), Q.push(norm.z);
			Q.push(x2), Q.push(y2), Q.push(z1), Q.push(0), Q.push(0), Q.push(norm.x), Q.push(norm.y), Q.push(norm.z);
		}
	}
	for (int i = 0; i < vertexCount; i++)
	{
		a[i] = Q.front();
		Q.pop();
	}
	return a;
}

float CUBE_VERTICES[] =
{
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f, 0.0f, -1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  0.0f, 0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f, 0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f, 0.0f, -1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f, 0.0f, -1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f, 0.0f, -1.0f,

	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f, 0.0f,  1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, 0.0f,  1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f, 0.0f,  1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f, 0.0f,  1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  0.0f, 0.0f,  1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f, 0.0f,  1.0f,

	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  -1.0f, 0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  -1.0f, 0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  -1.0f, 0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  -1.0f, 0.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  -1.0f, 0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  -1.0f, 0.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,   1.0f, 0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,   1.0f, 0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,   1.0f, 0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,   1.0f, 0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f, -1.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,  0.0f, -1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, -1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, -1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f, -1.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f, -1.0f, 0.0f,

	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  1.0f, 0.0f
};

float SKYBOX_VERTICES[] = {
	// positions
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	-1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	-1.0f,  1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f	
};

unsigned int CUBE_INDICES[] =
{
	0, 1, 2, 2, 3, 0,
	4, 5, 6, 5, 6, 7,
	3, 0, 4, 0, 4, 7,
	2, 1, 5, 1, 5, 6,
	0, 1, 5, 1, 5, 4,
	3, 2, 6, 2, 6, 7
};