#pragma once

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class VertexBuffer
{
public:
	VertexBuffer(const void* data, GLuint count, int size, GLenum type = GL_STATIC_DRAW);
	~VertexBuffer();
	void bind() const;
	void unbind() const;
	GLuint count() const { return m_VerticesCount; }
private:
	GLuint ID;
	GLuint m_VerticesCount;
};