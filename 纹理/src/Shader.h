#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

class Shader
{
public:
	GLuint ID;
public:
	Shader(const char* filepath);
	~Shader();
	void enable() const;
	void disable() const;
	void setUniform1i(const char* name, int v);
	void setUniform1f(const char* name, float v0);
	void setUniform2f(const char* name, float v0, float v1);
	void setUniform3f(const char* name, float v0, float v1, float v2);
	void setUniform4f(const char* name, float v0, float v1, float v2, float v3);
private:
	void compileShader(const char* vertexSource, const char* fragmentSource);
	GLint getUniformLocation(const char* name);
};