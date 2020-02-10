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
	void clear(float v0, float v1, float v2, float v3) const;
	void draw(const VertexArray& va, const IndexBuffer& eb, const Shader& shader) const;
};