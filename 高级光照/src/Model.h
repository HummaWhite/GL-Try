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
	void setScale(glm::vec3 scale);
	void setScale(float xScale, float yScale, float zScale);
	void setSize(float size);

	glm::vec3 pos() const { return m_Pos; }
	glm::vec3 scale() const { return m_Scale; }
	glm::mat4 modelMatrix() const;

	void loadShape(Shape& shape);

private:
	void processNode(aiNode* node, const aiScene* scene);
	Mesh* processMesh(aiMesh* mesh, const aiScene* scene);

private:
	std::vector<Mesh*> m_Meshes;
	glm::vec3 m_Pos;
	glm::vec3 m_Scale;
	glm::mat4 m_RotMatrix;
	bool m_LoadedFromFile;
	static glm::mat4 constRot;
};