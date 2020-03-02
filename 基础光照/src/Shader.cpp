#define _CRT_SECURE_NO_WARNINGS

#include "Shader.h"

#include <fstream>
#include <sstream>
#include <cstring>
#include <string>

Shader::Shader(const char* filepath)
	:ID(0)
{
	std::string vertexCode;
	std::string fragmentCode;
	std::string geometryCode;
	std::fstream file;
	bool includeGeomeryCode = 0;
	try
	{
		file.open(filepath);
		std::string fileString;
		while (std::getline(file, fileString))
		{
			if (fileString.find("//$Vertex") != fileString.npos) break;
		}
		while (std::getline(file, fileString))
		{
			if (fileString.find("//$Fragment") != fileString.npos) break;
			vertexCode += fileString + '\n';
		}
		if (vertexCode == "") throw "Error: vertex shader not found";
		while (std::getline(file, fileString))
		{
			if (fileString.find("//$Geometry") != fileString.npos)
			{
				includeGeomeryCode = 1;
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
		return;
	}

	const char* geometry = (includeGeomeryCode && (geometryCode != "")) ? geometryCode.c_str() : nullptr;
	compileShader(vertexCode.c_str(), fragmentCode.c_str(), geometry);
}

Shader::~Shader()
{
	glDeleteProgram(ID);
}

void Shader::enable() const
{
	glUseProgram(ID);
}

void Shader::disable() const
{
	glUseProgram(0);
}

void Shader::setUniform1i(const char* name, int v) const
{
	glUniform1i(getUniformLocation(name), v);
}

void Shader::setUniform1f(const char* name, float v0) const
{
	glUniform1f(getUniformLocation(name), v0);
}

void Shader::setUniform2f(const char* name, float v0, float v1) const
{
	glUniform2f(getUniformLocation(name), v0, v1);
}

void Shader::setUniform3f(const char* name, float v0, float v1, float v2) const
{
	glUniform3f(getUniformLocation(name), v0, v1, v2);
}

void Shader::setUniform4f(const char* name, float v0, float v1, float v2, float v3) const
{
	glUniform4f(getUniformLocation(name), v0, v1, v2, v3);
}

void Shader::setUniformVec3(const char* name, const glm::vec3 vec) const
{
	glUniform3f(getUniformLocation(name), vec.x, vec.y, vec.z);
}

void Shader::setUniformVec4(const char* name, const glm::vec4 vec) const
{
	glUniform4f(getUniformLocation(name), vec.x, vec.y, vec.z, vec.w);
}

void Shader::setUniformMat3(const char* name, const glm::mat3& mat) const
{
	glUniformMatrix3fv(getUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setUniformMat4(const char* name, const glm::mat4& mat) const
{
	glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setLight(const LightGroup& lightGroup)
{
	GLuint dirCount(0), pointCount(0), spotCount(0);
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
			sprintf(tmp, "pointLights[%d].attenuation", pointCount);
			setUniformVec3(tmp, light->attenuation);
			pointCount++;
			break;
		case Light::SPOT:
			sprintf(tmp, "spotLights[%d].pos", spotCount);
			setUniformVec3(tmp, light->pos);
			sprintf(tmp, "spotLights[%d].dir", spotCount);
			setUniformVec3(tmp, light->dir);
			sprintf(tmp, "spotLights[%d].color", spotCount);
			setUniformVec3(tmp, light->color);
			sprintf(tmp, "spotLights[%d].attenuation", spotCount);
			setUniformVec3(tmp, light->attenuation);
			sprintf(tmp, "spotLights[%d].cutOff", spotCount);
			setUniform1f(tmp, light->cutOff);
			sprintf(tmp, "spotLights[%d].outerCutOff", spotCount);
			setUniform1f(tmp, light->outerCutOff);
			spotCount++;
			break;
		}
	}
	if (dirCount > 0) setUniform1i("dirLightsCount", dirCount);
	if (pointCount > 0) setUniform1i("pointLightsCount", pointCount);
	if (spotCount > 0) setUniform1i("spotLightsCount", spotCount);
}

void Shader::setMaterial(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3 specular, float shininess)
{
	setUniformVec3("material.ambient", ambient);
	setUniformVec3("material.diffuse", diffuse);
	setUniformVec3("material.specular", specular);
	setUniform1f("material.shininess", shininess);
}

void Shader::useModelMatrix(const glm::mat4& model)
{
	setUniformMat4("model", model);
	glm::mat3 modelInv = glm::mat3(glm::transpose(glm::inverse(model)));
	setUniformMat3("modelInv", modelInv);
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

	ID = glCreateProgram();
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);
	if (geometrySource != nullptr)
		glAttachShader(ID, geometryShader);
	glLinkProgram(ID);

	GLint compileSuccess, linkSuccess;
	glGetProgramiv(ID, GL_COMPILE_STATUS, &compileSuccess);
	glGetProgramiv(ID, GL_LINK_STATUS, &linkSuccess);
	if ((!compileSuccess) || (!linkSuccess))
	{
		char info[512];
		glGetProgramInfoLog(ID, 512, NULL, info);
		std::cout << info << std::endl;
		return;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	if (geometrySource != nullptr)
		glDeleteShader(geometryShader);
}

GLint Shader::getUniformLocation(const char* name) const
{
	glUseProgram(ID);
	GLint location = glGetUniformLocation(ID, name);
	if (location == -1)
		std::cout << "Error: unable to locate the uniform::" << name << std::endl;
	return location;
}
