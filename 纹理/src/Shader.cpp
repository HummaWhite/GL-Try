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
	std::fstream file;
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
			fragmentCode += fileString + '\n';
		}
		if (fragmentCode == "") throw "Error: fragment shader not found";
		file.close();
	}
	catch (const char* err)
	{
		std::cout << err << std::endl;
		return;
	}

	compileShader(vertexCode.c_str(), fragmentCode.c_str());
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

void Shader::setUniformMat4(const char* name, const glm::mat4& mat) const
{
	glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
}

void Shader::compileShader(const char* vertexSource, const char* fragmentSource)
{
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);

	ID = glCreateProgram();
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);
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
}

GLint Shader::getUniformLocation(const char* name) const
{
	GLint location = glGetUniformLocation(ID, name);
	if (location == -1)
		std::cout << "Error: unable to locate the uniform::" << name << std::endl;
	return location;
}
