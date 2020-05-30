#include "Mesh.h"

void Mesh::loadMesh(const void* data, int count, std::vector<GLuint> indices, BufferLayout layout, bool batchedData)
{
	m_VB.allocate(count * layout.stride(), data, count, batchedData);
	m_VA.addBuffer(m_VB, layout);

	m_EB.allocate(indices.size() * sizeof(GLuint), &indices[0], indices.size());
}

void Mesh::addTexture(TextureMesh* tex)
{
	m_Textures.push_back(tex);
}

void Mesh::draw(Shader& shader)
{
	renderer.draw(m_VA, m_EB, shader);
}
