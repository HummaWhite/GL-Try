#pragma once

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Buffer
{
public:
	Buffer() : m_ID(0), m_VerticesCount(0) {}
	~Buffer();
	void loadData(const void* data, GLuint count, int size, GLenum type = GL_STATIC_DRAW);
	GLuint ID() const { return m_ID; }
	GLuint count() const { return m_VerticesCount; }
private:
	GLuint m_ID;
	GLuint m_VerticesCount;
};