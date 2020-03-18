#include "VertexBuffer.h"

VertexBuffer::VertexBuffer(const void* data, GLuint count, int size, GLenum type)
	:m_VerticesCount(count)
{
	glGenBuffers(1, &ID);
	glBindBuffer(GL_ARRAY_BUFFER, ID);
	glBufferData(GL_ARRAY_BUFFER, size, data, type);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

VertexBuffer::VertexBuffer(const Shape& shape) :
	m_VerticesCount(shape.vertexCount())
{
	if (shape.data() == nullptr)
	{
		std::cout << "Shape data not initilized" << std::endl;
		return;
	}
	glGenBuffers(1, &ID);
	glBindBuffer(GL_ARRAY_BUFFER, ID);
	glBufferData(GL_ARRAY_BUFFER, shape.size(), shape.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
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
