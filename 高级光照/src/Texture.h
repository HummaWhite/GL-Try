#pragma once

#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Texture
{
public:
	Texture();
	~Texture();

	GLuint ID() const { return m_ID; }
	GLuint type() const { return m_TextureType; }
	int width() const { return m_Width; }
	int height() const { return m_Height; }

	void generate2D(int width, int height, GLuint format, GLuint filterType = GL_LINEAR);
	void generateDepth2D(int width, int height, GLuint format, GLuint filterType = GL_LINEAR);
	void generateCube(int width, GLuint format, GLuint filterType = GL_LINEAR);
	void generateDepthCube(int width, GLuint filterType = GL_NEAREST);
	void loadSingle(const std::string& filePath, GLuint internalFormat = GL_RGBA, GLuint filterType = GL_LINEAR);
	void loadCube(const std::vector<std::string>& filePaths, GLuint internalFormat = GL_RGBA, GLuint filterType = GL_LINEAR);

	void bind() const;
	void bind(int slot) const;
	void unbind() const;

	int slot;

private:
	void allocate2D(GLuint internalFormat, int width, int height, GLuint sourceFormat, GLuint dataType, const void* data = nullptr);
	void allocateCube(GLuint internalFormat, int width, GLuint sourceFormat, GLuint dataType, const void* data = nullptr);

private:
	GLuint m_ID, m_TextureType;
	int m_Width, m_Height, m_BitsPerPixel;
	static int m_SlotsUsed;
};