#include "Buffer.h"

void Buffer::loadData(const void* data, GLuint count, int size, GLenum type)
{
	if (m_ID) return;
	glCreateBuffers(1, &m_ID);
	glNamedBufferData(m_ID, size, data, type);
	m_VerticesCount = size;
}

Buffer::~Buffer()
{
	glDeleteBuffers(1, &m_ID);
}
