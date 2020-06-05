#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Mesh.h"

class Model
{
public:
	Model();
	~Model();

	void loadModel(const char* filePath);
	void draw(Shader& shader);

	void setPos(glm::vec3 pos);
	void setPos(float x, float y, float z);
	void move(glm::vec3 vec);
	void rotateObjectSpace(float angle, glm::vec3 axis);
	void rotateWorldSpace(float abgle, glm::vec3 axis);

	glm::mat4 modelMatrix() const { return glm::translate(glm::mat4(1.0f), m_Pos) * m_RotMatrix * constRot; }

private:
	void processNode(aiNode* node, const aiScene* scene);
	Mesh* processMesh(aiMesh* mesh, const aiScene* scene);

private:
	std::vector<Mesh*> m_Meshes;
	glm::vec3 m_Pos;
	glm::mat4 m_RotMatrix;
	static glm::mat4 constRot;
};