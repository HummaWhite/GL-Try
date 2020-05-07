#pragma once

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Shader.h"
#include "Buffer.h"
#include "VertexArray.h"

class Renderer
{
public:
	void clear(float v0 = 1.0f, float v1 = 1.0f, float v2 = 1.0f, float v3 = 1.0f) const;
	void draw(const VertexArray& va, const Buffer& eb, const Shader& shader, GLuint renderMode = GL_FILL) const;
	void draw(const VertexArray& va, const Shader& shader, GLuint renderMode = GL_FILL) const;
};