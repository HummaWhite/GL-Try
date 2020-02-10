#include "Texture.h"
#include "stb_image/stb_image.h"

Texture::Texture(const std::string& filePath)
	:ID(0), m_FilePath(filePath), m_LocalBuffer(nullptr),
	m_Width(0), m_Height(0), m_BitsPerPixel(0)
{
	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_2D, ID);

	stbi_set_flip_vertically_on_load(1);
	m_LocalBuffer = stbi_load(filePath.c_str(), &m_Width, &m_Height, &m_BitsPerPixel, 4);
	if (m_LocalBuffer == nullptr)
		std::cout << "Error: unable to load texture: " << filePath << std::endl;

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer);
	glBindTexture(GL_TEXTURE_2D, 0);

	if (m_LocalBuffer != nullptr) stbi_image_free(m_LocalBuffer);
}

Texture::~Texture()
{
	glDeleteTextures(1, &ID);
}

void Texture::bind(GLuint slot) const
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, ID);
}

void Texture::unbind() const
{
	glBindTexture(GL_TEXTURE_2D, 0);
}