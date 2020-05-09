#pragma once

#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class FrameBuffer
{
public:
	FrameBuffer() : renderBufferExist(false) {}
	~FrameBuffer();
	void generate();

	void bind() const;
	void unbind() const;
	void attachRenderBuffer() const;
private:
	GLuint fbID, rbID;
	bool renderBufferExist;
};