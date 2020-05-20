#pragma once

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Lighting.h"
#include "Texture.h"

class Shader
{
public:
	Shader() : m_ID(0) {}
	~Shader();
	GLuint ID() const { return m_ID; }
	void load(const char* filePath);
	void enable() const;
	void disable() const;
	void setUniform1i(const char* name, int v) const;
	void setUniform1f(const char* name, float v0) const;
	void setUniform2f(const char* name, float v0, float v1) const;
	void setUniform3f(const char* name, float v0, float v1, float v2) const;
	void setUniform4f(const char* name, float v0, float v1, float v2, float v3) const;
	void setUniformVec3(const char* name, const glm::vec3& vec) const;
	void setUniformVec4(const char* name, const glm::vec4& vec) const;
	void setUniformMat3(const char* name, const glm::mat3& mat) const;
	void setUniformMat4(const char* name, const glm::mat4& mat) const;
	void setTexture(const char* name, const Texture& tex, int unit) const;
	void setLight(const LightGroup& lightGroup);
	void setMaterial(const glm::vec3& albedo, float metallic, float roughness, float ao);
	void useModelMatrix(const glm::mat4& model);
	static GLint getUniformLocation(GLuint programID, const char* name);
private:
	void compileShader(const char* vertexSource, const char* fragmentSource, const char* geometrySource);
	GLuint m_ID;
};