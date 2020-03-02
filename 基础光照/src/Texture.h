#pragma once

#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "FrameBuffer.h"

struct Material
{
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float shininess;
};

class Texture
{
public:
	Texture();
	~Texture();

	void loadSingle(const std::string& filePath);
	void loadCube(const std::vector<std::string>& filePaths);
	void attachDepthBufferCube(const FrameBuffer& frameBuffer);

	void bind() const;
	void bind(int slot) const;
	void unbind() const;

	int width() const { return m_Width; }
	int height() const { return m_Height; }

	static const int SHADOW_WIDTH = 1024;
	static const int SHADOW_HEIGHT = 1024;

	int slot;
private:
	GLuint ID, m_TextureType;
	int m_Width, m_Height, m_BitsPerPixel;
	bool m_Loaded;
	static int m_SlotsUsed;
};