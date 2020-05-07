#pragma once

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Buffer.h"
#include "BufferLayout.h"

class VertexArray
{
public:
	VertexArray();
	~VertexArray();

	void addBuffer(const Buffer& vb, const BufferLayout& layout);
	GLuint ID() const { return m_ID; }
	void bind() const;
	void unbind() const;
	GLuint count() const { return m_VerticesCount; }

private:
	GLuint m_ID;
	GLuint m_VerticesCount;
};