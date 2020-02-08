#pragma once

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class IndexBuffer
{
public:
	IndexBuffer(const void* data, int size, GLenum type = GL_STATIC_DRAW);
	~IndexBuffer();
	void bind();
	void unbind();
private:
	GLuint ID;
};