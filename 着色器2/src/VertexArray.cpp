#include "VertexArray.h"

VertexArray::VertexArray(void (*settings)())
{
	glGenVertexArrays(1, &ID);
	glBindVertexArray(ID);
	if (settings != nullptr) settings();
}

VertexArray::~VertexArray()
{
	glDeleteVertexArrays(1, &ID);
}

void VertexArray::bind()
{
	glBindVertexArray(ID);
}

void VertexArray::unbind()
{
	glBindVertexArray(0);
}