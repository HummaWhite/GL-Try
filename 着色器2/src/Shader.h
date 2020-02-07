#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

class Shader
{
public:
	Shader(const char* filepath);
	void enable();
	GLuint ID;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
};