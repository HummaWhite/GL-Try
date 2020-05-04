#include "Buffer.h"

Buffer::Buffer(const void* data, GLuint count, int size, GLenum type)
	:m_VerticesCount(count)
{
	glCreateBuffers(1, &m_ID);
	glNamedBufferData(m_ID, size, data, type);
}

Buffer::~Buffer()
{
	glDeleteBuffers(1, &m_ID);
}
