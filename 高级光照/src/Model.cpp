#include "Model.h"

Model::~Model()
{
	for (auto i : m_Meshes)
	{
		if (i != nullptr)
		{
			delete i;
		}
	}
}

void Model::loadModel(const char* filePath)
{
	Assimp::Importer importer;
	GLuint option = aiProcess_Triangulate
		| aiProcess_FlipUVs
		| aiProcess_GenSmoothNormals
		| aiProcess_GenUVCoords
		| aiProcess_FixInfacingNormals
		| aiProcess_FindInstances
		| aiProcess_JoinIdenticalVertices
		| aiProcess_OptimizeMeshes
		| aiProcess_ForceGenNormals
		| aiProcess_FindDegenerates;
	const aiScene* scene = importer.ReadFile(filePath, option);

	std::cout << "Model::Loading: " << filePath << std::endl;

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "Error: Assimp::" << importer.GetErrorString() << std::endl;
		return;
	}

	processNode(scene->mRootNode, scene);
}

void Model::draw(Shader& shader)
{
	for (auto i : m_Meshes)
	{
		i->draw(shader);
	}
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
	for (int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		m_Meshes.push_back(processMesh(mesh, scene));
	}

	for (int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

Mesh* Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<GLuint> indices;
	for (int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	std::vector<Mesh::POS3_UV2_NORM3> data;
	int validVertexCount = 0;
	for (int i = 0; i < mesh->mNumVertices; i++)
	{
		Mesh::POS3_UV2_NORM3 vertex =
		{
			{ mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z },
			{ 0.0f, 0.0f },
			{ mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z }
		};
		data.push_back(vertex);
	}

	Mesh* newMesh = new Mesh;
	newMesh->loadMesh(&data[0], mesh->mNumVertices, indices, LAYOUT_POS3_UV2_NORM3);

	return newMesh;
}
