#pragma once

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "VertexArray.h"
#include "Shader.h"

class Mesh
{
public:
	struct Vertex
	{
		glm::vec3 pos;
		glm::vec3 uv;
		glm::vec3 norm;
	};

	struct TextureMesh
	{
		unsigned int ID;
		std::string type;
	};

	Mesh();
	void loadMesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<TextureMesh> textures, const BufferLayout& layout);
	void setupMesh();
	void draw(Shader& shader);

private:
	VertexArray m_VA;
	Buffer m_VB, m_EB;
	std::vector<TextureMesh> m_Textures;
};