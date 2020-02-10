#pragma once

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class IndexBuffer
{
public:
	IndexBuffer(const GLuint* data, GLuint count, GLenum type = GL_STATIC_DRAW);
	~IndexBuffer();
	GLuint count() const;
	void bind() const;
	void unbind() const;
private:
	GLuint ID;
	GLuint m_Count;
};