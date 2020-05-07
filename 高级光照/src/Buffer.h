#pragma once

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Buffer
{
public:
	Buffer(const void* data, GLuint count, int size, GLenum type = GL_STATIC_DRAW);
	~Buffer();
	GLuint ID() const { return m_ID; }
	GLuint count() const { return m_VerticesCount; }
private:
	GLuint m_ID;
	GLuint m_VerticesCount;
};