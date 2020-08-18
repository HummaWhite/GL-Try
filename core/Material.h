#pragma once

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct MaterialPhong
{
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float shininess;
};

struct MaterialPBR
{
	glm::vec3 albedo;
	float metallic;
	float roughness;
	float ao;
};

namespace Material
{
	const MaterialPBR defaultMaterialPBR =
	{
		{ 0.3f, 0.6f, 1.0f }, 1.0f, 1.0f, 0.1f
	};
}