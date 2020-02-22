#include "VertexArray.h"

VertexArray::VertexArray()
	:m_VerticesCount(0)
{
	glGenVertexArrays(1, &ID);
	//glBindVertexArray(ID);
}

VertexArray::~VertexArray()
{
	glDeleteVertexArrays(1, &ID);
}

void VertexArray::addBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout)
{
	bind();
	vb.bind();
	const auto& elements = layout.elements();
	GLuint offset = 0;

	for (int i = 0; i < elements.size(); i++)
	{
		glEnableVertexAttribArray(i);
		int count = elements[i].count;
		GLuint type = elements[i].type;
		bool normalized = elements[i].normalized;
		glVertexAttribPointer(i, count, type, normalized, layout.stride(), (void*)offset);
		offset += sizeofGLType(type) * count;
	}
	m_VerticesCount = vb.count();
}

void VertexArray::bind() const
{
	glBindVertexArray(ID);
}

void VertexArray::unbind() const
{
	glBindVertexArray(0);
}