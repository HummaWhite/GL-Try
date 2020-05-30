#pragma once

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Buffer
{
public:
	Buffer() : m_ID(0), m_ElementsCount(0), m_Size(0), m_Batched(false) {}
	~Buffer();

	void allocate(int size, const void* data, GLuint elementsCount, bool batched = false, GLenum type = GL_STATIC_DRAW);
	void write(int offset, int size, const void* data);

	GLuint ID() const { return m_ID; }
	GLuint elementsCount() const { return m_ElementsCount; }
	GLuint size() const { return m_Size; }
	bool batched() const { return m_Batched; }

private:
	GLuint m_ID;
	GLuint m_ElementsCount;
	GLuint m_Size;
	bool m_Batched;
};