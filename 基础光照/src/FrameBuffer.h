#pragma once

#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class FrameBuffer
{
public:
	FrameBuffer();
	~FrameBuffer();

	void bind() const;
	void unbind() const;
private:
	GLuint ID;
};