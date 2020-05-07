#include "VertexArray.h"

VertexArray::VertexArray()
	:m_VerticesCount(0)
{
	glCreateVertexArrays(1, &m_ID);
}

VertexArray::~VertexArray()
{
	glDeleteVertexArrays(1, &m_ID);
}

void VertexArray::addBuffer(const Buffer& vb, const BufferLayout& layout)
{
	this->bind();
	const auto& elements = layout.elements();
	GLuint offset = 0;
	GLuint bindingIndex = 0;

	for (int i = 0; i < elements.size(); i++)
	{
		glEnableVertexArrayAttrib(m_ID, i);
		int count = elements[i].count;
		GLuint type = elements[i].type;
		bool normalized = elements[i].normalized;
		glVertexArrayAttribFormat(m_ID, i, count, type, normalized, offset);
		glVertexArrayAttribBinding(m_ID, i, bindingIndex);
		offset += sizeofGLType(type) * count;
	}
	glVertexArrayVertexBuffer(m_ID, bindingIndex, vb.ID(), 0, offset);
	m_VerticesCount = vb.count();
	this->unbind();
}

void VertexArray::bind() const
{
	glBindVertexArray(m_ID);
}

void VertexArray::unbind() const
{
	glBindVertexArray(0);
}