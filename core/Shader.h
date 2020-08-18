#pragma once

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <map>

#include "Lighting.h"
#include "Texture.h"
#include "Material.h"

class Shader
{
public:
	Shader() : m_ID(0) {}
	Shader(const char* filePath);
	~Shader();
	GLuint ID() const { return m_ID; }
	bool load(const char* filePath);
	void enable() const;
	void disable() const;
	void set1i(const char* name, int v) const;
	void set1f(const char* name, float v0) const;
	void set2f(const char* name, float v0, float v1) const;
	void set3f(const char* name, float v0, float v1, float v2) const;
	void set4f(const char* name, float v0, float v1, float v2, float v3) const;
	void set1d(const char* name, double v) const;
	void setVec2(const char* name, const glm::vec2& vec) const;
	void setVec3(const char* name, const glm::vec3& vec) const;
	void setVec4(const char* name, const glm::vec4& vec) const;
	void setMat3(const char* name, const glm::mat3& mat) const;
	void setMat4(const char* name, const glm::mat4& mat) const;
	void setVec2d(const char* name, const glm::dvec2& vec) const;
	void setTexture(const char* name, const Texture& tex);
	void setLight(const std::vector<Light*> & lightGroup);
	void setMaterial(const glm::vec3& albedo, float metallic, float roughness, float ao);
	void setMaterial(const MaterialPhong& material);
	void setMaterial(const MaterialPBR& material);
	static GLint getUniformLocation(GLuint programID, const char* name);

	std::string name() const { return m_Name; }

	void resetTextureMap();

private:
	void compileShader(const char* vertexSource, const char* fragmentSource, const char* geometrySource);
	GLuint m_ID;
	std::string m_Name;

	std::map<std::string, int> m_TextureMap;
};