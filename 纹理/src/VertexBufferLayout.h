#pragma once

#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "GLSizeofType.h"

struct VertexBufferElement
{
	GLuint type;
	int count;
	bool normalized;
};

class VertexBufferLayout
{
private:
	std::vector<VertexBufferElement> m_Elements;
	GLuint m_Stride;
public:
	VertexBufferLayout() : m_Stride(0) {};
	~VertexBufferLayout() {};

	std::vector<VertexBufferElement> elements() const
	{
		return m_Elements;
	}
	GLuint stride() const
	{
		return m_Stride;
	}

	template<typename T>
	void add(int count, bool normalized = GL_FALSE)
	{
		static_assert(false);
	}

	template<>
	void add<float>(int count, bool normalized)
	{
		m_Elements.push_back({ GL_FLOAT, count, normalized });
		m_Stride += count * sizeof(float);
	}

	template<>
	void add<GLuint>(int count, bool normalized)
	{
		m_Elements.push_back({ GL_UNSIGNED_INT, count, normalized });
		m_Stride += count * sizeof(GLuint);
	}

	template<>
	void add<int>(int count, bool normalized)
	{
		m_Elements.push_back({ GL_INT, count, normalized });
		m_Stride += count * sizeof(int);
	}

	template<>
	void add<GLbyte>(int count, bool normalized)
	{
		m_Elements.push_back({ GL_BYTE, count, normalized });
		m_Stride += count * sizeof(GLbyte);
	}

	template<GLuint T>
	void add(int count, bool normalized = 0)
	{
		m_Elements.push_back({ T, count, normalized });
		m_Stride += count * sizeofGLType(T);
	}
};