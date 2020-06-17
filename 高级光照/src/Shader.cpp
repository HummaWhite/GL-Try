#define _CRT_SECURE_NO_WARNINGS

#include "Shader.h"

#include <fstream>
#include <sstream>
#include <cstring>
#include <string>

bool Shader::load(const char* filePath)
{
	std::string vertexCode;
	std::string fragmentCode;
	std::string geometryCode;
	std::fstream file;
	bool includeGeomeryCode = 0;

	std::cout << "Loading Shader: " << filePath << " ... ";

	try
	{
		file.open(filePath);
		if (!file.is_open())
		{
			std::cout << "Shader file not found" << std::endl;
			return false;
		}
		std::string fileString;
		while (std::getline(file, fileString))
		{
			if (fileString == "//$Vertex") break;
		}
		while (std::getline(file, fileString))
		{
			if (fileString == "//$Fragment") break;
			vertexCode += fileString + '\n';
		}
		if (vertexCode == "") throw "Error: vertex shader not found";
		while (std::getline(file, fileString))
		{
			if (fileString == "//$Geometry")
			{
				includeGeomeryCode = true;
				break;
			}
			fragmentCode += fileString + '\n';
		}
		if (fragmentCode == "") throw "Error: fragment shader not found";
		while (std::getline(file, fileString))
		{
			geometryCode += fileString + '\n';
		}
		if (includeGeomeryCode && geometryCode == "") throw "Error: geometry shader not found";
		file.close();
	}
	catch (const char* err)
	{
		std::cout << err << std::endl;
		exit(-1);
	}

	const char* geometry = (includeGeomeryCode && (geometryCode != "")) ? geometryCode.c_str() : nullptr;
	compileShader(vertexCode.c_str(), fragmentCode.c_str(), geometry);

	std::cout << "Done" << std::endl;
	m_Name = std::string(filePath);
	return true;
}

Shader::Shader(const char* filePath)
{
	load(filePath);
}

Shader::~Shader()
{
	glDeleteProgram(m_ID);
}

void Shader::enable() const
{
	glUseProgram(m_ID);
}

void Shader::disable() const
{
	glUseProgram(0);
}

void Shader::setUniform1i(const char* name, int v) const
{
	glProgramUniform1i(m_ID, getUniformLocation(m_ID, name), v);
}

void Shader::setUniform1f(const char* name, float v0) const
{
	glProgramUniform1f(m_ID, getUniformLocation(m_ID, name), v0);
}

void Shader::setUniform2f(const char* name, float v0, float v1) const
{
	glProgramUniform2f(m_ID, getUniformLocation(m_ID, name), v0, v1);
}

void Shader::setUniform3f(const char* name, float v0, float v1, float v2) const
{
	glProgramUniform3f(m_ID, getUniformLocation(m_ID, name), v0, v1, v2);
}

void Shader::setUniform4f(const char* name, float v0, float v1, float v2, float v3) const
{
	glProgramUniform4f(m_ID, getUniformLocation(m_ID, name), v0, v1, v2, v3);
}

void Shader::setUniformVec3(const char* name, const glm::vec3& vec) const
{
	glProgramUniform3f(m_ID, getUniformLocation(m_ID, name), vec.x, vec.y, vec.z);
}

void Shader::setUniformVec4(const char* name, const glm::vec4& vec) const
{
	glProgramUniform4f(m_ID, getUniformLocation(m_ID, name), vec.x, vec.y, vec.z, vec.w);
}

void Shader::setUniformMat3(const char* name, const glm::mat3& mat) const
{
	glProgramUniformMatrix3fv(m_ID, getUniformLocation(m_ID, name), 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::setUniformMat4(const char* name, const glm::mat4& mat) const
{
	glProgramUniformMatrix4fv(m_ID, getUniformLocation(m_ID, name), 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::setTexture(const char* name, const Texture& tex, int unit) const
{
	glBindTextureUnit(unit, tex.ID());
	setUniform1i(name, unit);
}

void Shader::setLight(const std::vector<Light*>& lightGroup)
{
	GLuint dirCount(0), pointCount(0);
	char tmp[50];
	for (auto light : lightGroup)
	{
		if (light == nullptr) continue;
		switch (light->type)
		{
		case Light::DIRECTIONAL:
			sprintf(tmp, "dirLights[%d].dir", dirCount);
			setUniformVec3(tmp, light->dir);
			sprintf(tmp, "dirLights[%d].color", dirCount);
			setUniformVec3(tmp, light->color);
			dirCount++;
			break;
		case Light::POINT:
			sprintf(tmp, "pointLights[%d].pos", pointCount);
			setUniformVec3(tmp, light->pos);
			sprintf(tmp, "pointLights[%d].dir", pointCount);
			setUniformVec3(tmp, light->dir);
			sprintf(tmp, "pointLights[%d].color", pointCount);
			setUniformVec3(tmp, light->color);
			sprintf(tmp, "pointLights[%d].cutoff", pointCount);
			setUniform1f(tmp, cos(glm::radians(light->cutoff)));
			sprintf(tmp, "pointLights[%d].outerCutoff", pointCount);
			setUniform1f(tmp, cos(glm::radians(light->outerCutoff)));
			sprintf(tmp, "pointLights[%d].attenuation", pointCount);
			setUniformVec3(tmp, light->attenuation);
			sprintf(tmp, "pointLights[%d].strength", pointCount);
			setUniform1f(tmp, exp(light->strength));
			sprintf(tmp, "pointLights[%d].size", pointCount);
			setUniform1f(tmp, light->size);
			pointCount++;
			break;
		}
	}
	if (dirCount > 0) setUniform1i("dirLightsCount", dirCount);
	if (pointCount > 0) setUniform1i("pointLightsCount", pointCount);
}

void Shader::setMaterial(const glm::vec3& albedo, float metallic, float roughness, float ao)
{
	setUniformVec3("material.albedo", albedo);
	setUniform1f("material.metallic", metallic);
	setUniform1f("material.roughness", roughness);
	setUniform1f("material.ao", ao);
}

void Shader::setMaterial(const MaterialPhong& material)
{
	setUniformVec3("material.ambient", material.ambient);
	setUniformVec3("material.diffuse", material.diffuse);
	setUniformVec3("material.specular", material.specular);
	setUniform1f("material.shininess", material.shininess);
}

void Shader::setMaterial(const MaterialPBR& material)
{
	setUniformVec3("material.albedo", material.albedo);
	setUniform1f("material.metallic", material.metallic);
	setUniform1f("material.roughness", material.roughness);
	setUniform1f("material.ao", material.ao);
}

GLint Shader::getUniformLocation(GLuint programID, const char* name)
{
	GLint location = glGetUniformLocation(programID, name);
	if (location == -1)
		std::cout << "Error: unable to locate the uniform::" << name << " in shader number: " << programID << std::endl;
	return location;
}

void Shader::compileShader(const char* vertexSource, const char* fragmentSource, const char* geometrySource)
{
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);

	GLuint geometryShader;
	if (geometrySource != nullptr)
	{
		geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometryShader, 1, &geometrySource, NULL);
		glCompileShader(geometryShader);
	}

	m_ID = glCreateProgram();
	glAttachShader(m_ID, vertexShader);
	glAttachShader(m_ID, fragmentShader);
	if (geometrySource != nullptr)
		glAttachShader(m_ID, geometryShader);
	glLinkProgram(m_ID);

	GLint compileSuccess, linkSuccess;
	glGetProgramiv(m_ID, GL_COMPILE_STATUS, &compileSuccess);
	glGetProgramiv(m_ID, GL_LINK_STATUS, &linkSuccess);
	if ((!compileSuccess) || (!linkSuccess))
	{
		char info[512];
		glGetProgramInfoLog(m_ID, 512, NULL, info);
		std::cout << info << std::endl;
		exit(-1);
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	if (geometrySource != nullptr)
		glDeleteShader(geometryShader);
}
