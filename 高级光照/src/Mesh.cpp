#include "Mesh.h"

void Mesh::loadMesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<TextureMesh> textures, const BufferLayout& layout)
{
	m_Textures = textures;
}
