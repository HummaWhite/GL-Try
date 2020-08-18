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

void Shader::set1i(const char* name, int v) const
{
	glProgramUniform1i(m_ID, getUniformLocation(m_ID, name), v);
}

void Shader::set1f(const char* name, float v0) const
{
	glProgramUniform1f(m_ID, getUniformLocation(m_ID, name), v0);
}

void Shader::set2f(const char* name, float v0, float v1) const
{
	glProgramUniform2f(m_ID, getUniformLocation(m_ID, name), v0, v1);
}

void Shader::set3f(const char* name, float v0, float v1, float v2) const
{
	glProgramUniform3f(m_ID, getUniformLocation(m_ID, name), v0, v1, v2);
}

void Shader::set4f(const char* name, float v0, float v1, float v2, float v3) const
{
	glProgramUniform4f(m_ID, getUniformLocation(m_ID, name), v0, v1, v2, v3);
}

void Shader::set1d(const char* name, double v) const
{
	glProgramUniform1d(m_ID, getUniformLocation(m_ID, name), v);
}

void Shader::setVec3(const char* name, const glm::vec3& vec) const
{
	glProgramUniform3f(m_ID, getUniformLocation(m_ID, name), vec.x, vec.y, vec.z);
}

void Shader::setVec4(const char* name, const glm::vec4& vec) const
{
	glProgramUniform4f(m_ID, getUniformLocation(m_ID, name), vec.x, vec.y, vec.z, vec.w);
}

void Shader::setMat3(const char* name, const glm::mat3& mat) const
{
	glProgramUniformMatrix3fv(m_ID, getUniformLocation(m_ID, name), 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::setMat4(const char* name, const glm::mat4& mat) const
{
	glProgramUniformMatrix4fv(m_ID, getUniformLocation(m_ID, name), 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::setVec2d(const char* name, const glm::dvec2& vec) const
{
	glProgramUniform2dv(m_ID, getUniformLocation(m_ID, name), 1, glm::value_ptr(vec));
}

void Shader::setTexture(const char* name, const Texture& tex)
{
	std::string texName(name);
	std::map<std::string, int>::iterator it = m_TextureMap.find(texName);

	int texUnit = 0;
	if (it == m_TextureMap.end())
	{
		texUnit = m_TextureMap.size();
		m_TextureMap[texName] = texUnit;
	}
	
	glBindTextureUnit(texUnit, tex.ID());
	set1i(name, texUnit);
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
			setVec3(tmp, light->dir);
			sprintf(tmp, "dirLights[%d].color", dirCount);
			setVec3(tmp, light->color);
			dirCount++;
			break;
		case Light::POINT:
			sprintf(tmp, "pointLights[%d].pos", pointCount);
			setVec3(tmp, light->pos);
			sprintf(tmp, "pointLights[%d].dir", pointCount);
			setVec3(tmp, light->dir);
			sprintf(tmp, "pointLights[%d].color", pointCount);
			setVec3(tmp, light->color);
			sprintf(tmp, "pointLights[%d].cutoff", pointCount);
			set1f(tmp, cos(glm::radians(light->cutoff)));
			sprintf(tmp, "pointLights[%d].outerCutoff", pointCount);
			set1f(tmp, cos(glm::radians(light->outerCutoff)));
			sprintf(tmp, "pointLights[%d].attenuation", pointCount);
			setVec3(tmp, light->attenuation);
			sprintf(tmp, "pointLights[%d].strength", pointCount);
			set1f(tmp, exp(light->strength));
			sprintf(tmp, "pointLights[%d].size", pointCount);
			set1f(tmp, light->size);
			pointCount++;
			break;
		}
	}
	if (dirCount > 0) set1i("dirLightsCount", dirCount);
	if (pointCount > 0) set1i("pointLightsCount", pointCount);
}

void Shader::setMaterial(const glm::vec3& albedo, float metallic, float roughness, float ao)
{
	setVec3("material.albedo", albedo);
	set1f("material.metallic", metallic);
	set1f("material.roughness", roughness);
	set1f("material.ao", ao);
}

void Shader::setMaterial(const MaterialPhong& material)
{
	setVec3("material.ambient", material.ambient);
	setVec3("material.diffuse", material.diffuse);
	setVec3("material.specular", material.specular);
	set1f("material.shininess", material.shininess);
}

void Shader::setMaterial(const MaterialPBR& material)
{
	setVec3("material.albedo", material.albedo);
	set1f("material.metallic", material.metallic);
	set1f("material.roughness", material.roughness);
	set1f("material.ao", material.ao);
}

GLint Shader::getUniformLocation(GLuint programID, const char* name)
{
	GLint location = glGetUniformLocation(programID, name);
	if (location == -1)
		std::cout << "Error: unable to locate the uniform::" << name << " in shader number: " << programID << std::endl;
	return location;
}

void Shader::resetTextureMap()
{
	m_TextureMap.clear();
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
