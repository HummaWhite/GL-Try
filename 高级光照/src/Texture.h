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
	GLuint ID() const { return m_ID; }

	void loadSingle(const std::string& filePath, GLuint internalType = GL_RGBA, GLuint filterType = GL_LINEAR);
	void loadCube(const std::vector<std::string>& filePaths, GLuint internalType = GL_RGBA, GLuint filterType = GL_LINEAR);
	void attachDepthBufferCube(const FrameBuffer& frameBuffer, int resolution);
	void attachColorBufferCube(const FrameBuffer& frameBuffer, int resolution, GLuint colorFormat = GL_RGB);
	void attachFrameBuffer2D(const FrameBuffer& frameBuffer, GLuint type, int width, int height);
	void attachColorBuffer2D(const FrameBuffer& frameBuffer, int width, int height, GLuint colorFormat = GL_RGB);

	void bind() const;
	void bind(int slot) const;
	void unbind() const;

	int width() const { return m_Width; }
	int height() const { return m_Height; }

	int slot;
private:
	GLuint m_ID, m_TextureType;
	int m_Width, m_Height, m_BitsPerPixel;
	static int m_SlotsUsed;
};