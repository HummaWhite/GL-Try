#include "Renderer.h"

void Renderer::clear(float v0, float v1, float v2, float v3) const
{
	glClearColor(v0, v1, v2, v3);
	glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::draw(const VertexArray& va, const IndexBuffer& eb, const Shader& shader) const
{
	shader.enable();
	va.bind();
	eb.bind();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawElements(GL_TRIANGLES, eb.count(), GL_UNSIGNED_INT, 0);
}
