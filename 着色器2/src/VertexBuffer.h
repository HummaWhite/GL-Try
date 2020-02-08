#pragma once

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class VertexBuffer
{
public:
	VertexBuffer(const void* data, int size, GLenum type = GL_STATIC_DRAW);
	~VertexBuffer();
	void bind();
	void unbind();
private:
	GLuint ID;
};