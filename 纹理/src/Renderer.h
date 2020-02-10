#pragma once

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Shader.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"

class Renderer
{
public:
	void clear(float v0 = 1.0f, float v1 = 1.0f, float v2 = 1.0f, float v3 = 1.0f) const;
	void draw(const VertexArray& va, const IndexBuffer& eb, const Shader& shader) const;
};