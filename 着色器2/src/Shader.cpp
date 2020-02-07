#include "Shader.h"

#include <fstream>
#include <sstream>
#include <cstring>
#include <string>

Shader::Shader(const char* filepath)
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

	const char* vertexSource = vertexCode.c_str();
	const char* fragmentSource = fragmentCode.c_str();

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
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void Shader::enable()
{
	glUseProgram(ID);
}

void Shader::setInt(const std::string& name, int value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) const
{
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}
