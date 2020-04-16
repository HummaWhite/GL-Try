#pragma once
#pragma GCC optimize(3, "Ofast", "inline")

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "VertexBufferLayout.h"

#include <iostream>
#include <cmath>

class Shape
{
public:
	Shape(int vertexCount, int type);
	float* data() const { return m_Buffer; }
	int type() const { return m_Type; }
	int vertexCount() const { return m_VertexCount; }
	VertexBufferLayout layout() const { return m_Layout; }
	int size() const { return m_VertexCount * m_Layout.stride(); }
	void addTangents();
	void setBuffer(float* buffer);
	enum
	{
		CUBE = 1,
		SQUARE,
		CONE,
		SPHERE,
		TORUS,
		BEZIER,
		SPECIAL
	};
private:
	float* m_Buffer;
	int m_Type;
	bool m_WithTangents;
	int m_VertexCount;
	VertexBufferLayout m_Layout;
};

class Cube :
	public Shape
{
public:
	Cube();
};

class Square :
	public Shape
{
public:
	Square();
};

class Cone :
	public Shape
{
public:
	Cone(int faces, float radius, float height);
};

class Sphere :
	public Shape
{
public:
	Sphere(int columns, int rows, float radius, float Atheta = 360.0f, float Arho = 180.0f);
};

class Torus :
	public Shape
{
public:
	Torus(int columns, int rows, float majorRadius, float minorRadius, float Atheta = 360.0f, float Btheta = 360.0f);
};

class Bezier :
	public Shape
{
public:
	Bezier(int _n, int _m, int _secU, int _secV, const std::vector<glm::vec3>& points);
	int n, m, secU, secV;
};

const float CUBE_VERTICES[] =
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

const float SKYBOX_VERTICES[] = {
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

const unsigned int CUBE_INDICES[] =
{
	0, 1, 2, 2, 3, 0,
	4, 5, 6, 5, 6, 7,
	3, 0, 4, 0, 4, 7,
	2, 1, 5, 1, 5, 6,
	0, 1, 5, 1, 5, 4,
	3, 2, 6, 2, 6, 7
};

const float SQUARE_VERTICES[] =
{
	 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
	-1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
	1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
	1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
	-1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
	-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f
};

const float SCREEN_COORD[] =
{
	0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f
};