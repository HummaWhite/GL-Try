#pragma once

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class VertexArray
{
public:
	VertexArray(void (*settings)() = nullptr);
	~VertexArray();

	void bind();
	void unbind();

private:
	GLuint ID;
};