#include "SceneLoader.h"

#include <fstream>

void SceneLoader::loadScene(Engine* engine, const char* filePath)
{
	if (engine == nullptr) return;
	std::fstream file(filePath);
	std::cout << "Loading Scene: " << filePath << std::endl;

	if (!file.is_open())
	{
		std::cout << "Error: Scene file not found" << std::endl;
		exit(-1);
	}

	std::string section;
	while (std::getline(file, section))
	{
		if (section == "#Objects") break;
	}

	int objectsCount;
	file >> objectsCount;
	for (int i = 0; i < objectsCount; i++)
	{
		std::string modelPath, tmp;
		glm::vec3 modelPos(0.0f);
		float modelSize(1.0f);
		MaterialPBR modelMaterial(Material::defaultMaterialPBR);
		file >> modelPath;
		file >> tmp;
		if (tmp == "P")
		{
			file >> modelPos.x >> modelPos.y >> modelPos.z;
		}
		file >> tmp;
		if (tmp == "S")
		{
			file >> modelSize;
		}
		file >> tmp;
		if (tmp == "M")
		{
			file >> modelMaterial.albedo.r >> modelMaterial.albedo.g >> modelMaterial.albedo.b;
			file >> modelMaterial.metallic >> modelMaterial.roughness >> modelMaterial.ao;
		}
		Model* model = new Model(modelPath.c_str(), modelPos, modelSize);
		engine->addObject(model, modelMaterial);
	}

	while (std::getline(file, section))
	{
		std::cout << section << std::endl;
		if (section == "#Lights") break;
	}

	int lightCount;
	file >> lightCount;
	for (int i = 0; i < lightCount; i++)
	{
		glm::vec3 lightPos, lightColor;
		file >> lightPos.x >> lightPos.y >> lightPos.z;
		file >> lightColor.x >> lightColor.y >> lightColor.z;
		Light* light = new Light(lightPos, lightColor);
		engine->addLight(light);
	}

	file.close();
}
