#include "VertexBuffer.h"

VertexBuffer::VertexBuffer(const void* data, int size, GLenum type)
{
	glGenBuffers(1, &ID);
	glBindBuffer(GL_ARRAY_BUFFER, ID);
	glBufferData(GL_ARRAY_BUFFER, size, data, type);
}

VertexBuffer::~VertexBuffer()
{
	glDeleteBuffers(1, &ID);
}

void VertexBuffer::bind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, ID);
}

void VertexBuffer::unbind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
