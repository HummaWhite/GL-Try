#pragma once

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Lighting.h"

class Shader
{
public:
	GLuint ID;
public:
	Shader(const char* filepath);
	~Shader();
	void enable() const;
	void disable() const;
	void setUniform1i(const char* name, int v) const;
	void setUniform1f(const char* name, float v0) const;
	void setUniform2f(const char* name, float v0, float v1) const;
	void setUniform3f(const char* name, float v0, float v1, float v2) const;
	void setUniform4f(const char* name, float v0, float v1, float v2, float v3) const;
	void setUniformVec3(const char* name, const glm::vec3 vec) const;
	void setUniformVec4(const char* name, const glm::vec4 vec) const;
	void setUniformMat3(const char* name, const glm::mat3& mat) const;
	void setUniformMat4(const char* name, const glm::mat4& mat) const;
	void setLight(const LightGroup& lightGroup);
	void setMaterial(const glm::vec3& ambient, const glm::vec3& diffuse,
		const glm::vec3 specular, float shininess);
private:
	void compileShader(const char* vertexSource, const char* fragmentSource, const char* geometrySource);
	GLint getUniformLocation(const char* name) const;
};