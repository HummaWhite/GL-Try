#pragma once

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Texture
{
public:
	Texture(const std::string& filePath);
	~Texture();

	void bind(GLuint slot = 0) const;
	void unbind() const;

	int width() const { return m_Width; }
	int height() const { return m_Height; }
private:
	GLuint ID;
	std::string m_FilePath;
	GLubyte* m_LocalBuffer;
	int m_Width, m_Height, m_BitsPerPixel;
};