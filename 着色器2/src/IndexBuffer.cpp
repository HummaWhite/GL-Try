#include "IndexBuffer.h"

IndexBuffer::IndexBuffer(const GLuint* data, GLuint count, GLenum type)
	:m_Count(count)
{
	glGenBuffers(1, &ID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(GLuint), data, type);
}

IndexBuffer::~IndexBuffer()
{
	glDeleteBuffers(1, &ID);
}

GLuint IndexBuffer::count() const
{
	return m_Count;
}

void IndexBuffer::bind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
}

void IndexBuffer::unbind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}