#include "Texture.h"
#include "stb_image/stb_image.h"

int Texture::m_SlotsUsed = 0;

Texture::Texture() :
	m_ID(0), m_Width(0), m_Height(0), m_BitsPerPixel(0), m_TextureType(0)
{
}

Texture::~Texture()
{
	glDeleteTextures(1, &m_ID);
}

void Texture::generate2D(int width, int height, GLuint format, GLuint filterType)
{
	m_TextureType = GL_TEXTURE_2D;

	glCreateTextures(GL_TEXTURE_2D, 1, &m_ID);
	allocate2D(format, width, height, GL_RGB, GL_FLOAT);

	glTextureParameteri(m_ID, GL_TEXTURE_MIN_FILTER, filterType);
	glTextureParameteri(m_ID, GL_TEXTURE_MAG_FILTER, filterType);

	slot = m_SlotsUsed++;
}

void Texture::generateDepth2D(int width, int height, GLuint format, GLuint filterType)
{
	m_TextureType = GL_TEXTURE_2D;

	glCreateTextures(GL_TEXTURE_2D, 1, &m_ID);
	allocate2D(GL_DEPTH_COMPONENT, width, height, GL_DEPTH_COMPONENT, GL_FLOAT);

	glTextureParameteri(m_ID, GL_TEXTURE_MIN_FILTER, filterType);
	glTextureParameteri(m_ID, GL_TEXTURE_MAG_FILTER, filterType);

	slot = m_SlotsUsed++;
}

void Texture::generateCube(int width, GLuint format, GLuint filterType)
{
	m_TextureType = GL_TEXTURE_CUBE_MAP;

	glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_ID);
	allocateCube(format, width, GL_RGB, GL_FLOAT);

	glTextureParameteri(m_ID, GL_TEXTURE_MIN_FILTER, filterType);
	glTextureParameteri(m_ID, GL_TEXTURE_MAG_FILTER, filterType);
	glTextureParameteri(m_ID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(m_ID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureParameteri(m_ID, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	slot = m_SlotsUsed++;
}

void Texture::generateDepthCube(int width, GLuint filterType)
{
	m_TextureType = GL_TEXTURE_CUBE_MAP;

	glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_ID);
	allocateCube(GL_DEPTH_COMPONENT, width, GL_DEPTH_COMPONENT, GL_FLOAT);

	glTextureParameteri(m_ID, GL_TEXTURE_MIN_FILTER, filterType);
	glTextureParameteri(m_ID, GL_TEXTURE_MAG_FILTER, filterType);
	glTextureParameteri(m_ID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(m_ID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureParameteri(m_ID, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	slot = m_SlotsUsed++;
}

void Texture::loadSingle(const std::string& filePath, GLuint internalFormat, GLuint filterType)
{
	m_TextureType = GL_TEXTURE_2D;

	//stbi_set_flip_vertically_on_load(1);
	GLubyte* data = stbi_load(filePath.c_str(), &m_Width, &m_Height, &m_BitsPerPixel, 4);
	if (data == nullptr)
	{
		std::cout << "Error: unable to load texture: " << filePath << std::endl;
		return;
	}

	glCreateTextures(GL_TEXTURE_2D, 1, &m_ID);
	allocate2D(internalFormat, m_Width, m_Height, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glTextureParameteri(m_ID, GL_TEXTURE_MIN_FILTER, filterType);
	glTextureParameteri(m_ID, GL_TEXTURE_MAG_FILTER, filterType);
	glTextureParameteri(m_ID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(m_ID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	if (data != nullptr) stbi_image_free(data);
	slot = m_SlotsUsed++;
}

void Texture::loadCube(const std::vector<std::string>& filePaths, GLuint internalFormat, GLuint filterType)
{
	m_TextureType = GL_TEXTURE_CUBE_MAP;

	//stbi_set_flip_vertically_on_load(1);

	GLubyte* data[6];
	for (int i = 0; i < filePaths.size(); i++)
	{
		data[i] = stbi_load(filePaths[i].c_str(), &m_Width, &m_Height, &m_BitsPerPixel, 4);
		if (data == nullptr)
		{
			std::cout << "Error: unable to load texture: " << filePaths[i] << std::endl;
			return;
		}
	}

	for (int i = 0; i < filePaths.size(); i++)
	{
		glTextureImage2DEXT
		(
			m_ID, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
			0, internalFormat, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data
		);
		allocateCube(internalFormat, m_Width, GL_RGBA, GL_UNSIGNED_BYTE, data);
		stbi_image_free(data);
	}

	glTextureParameteri(m_ID, GL_TEXTURE_MIN_FILTER, filterType);
	glTextureParameteri(m_ID, GL_TEXTURE_MAG_FILTER, filterType);
	glTextureParameteri(m_ID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(m_ID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureParameteri(m_ID, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	slot = m_SlotsUsed++;
}

void Texture::bind() const
{
	glActiveTexture(GL_TEXTURE0 + this->slot);
	//glBindTextureUnit(GL_TEXTURE0 + this->slot, m_ID);
	glBindTexture(m_TextureType, m_ID);
}

void Texture::bind(int slot) const
{
	//glBindTextureUnit(GL_TEXTURE0 + slot, m_ID);
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(m_TextureType, m_ID);
}

void Texture::unbind() const
{
	glBindTexture(m_TextureType, 0);
}

void Texture::allocate2D(GLuint internalFormat, int width, int height, GLuint sourceFormat, GLuint dataType, const void* data)
{
	glTextureImage2DEXT(m_ID, GL_TEXTURE_2D, 0, internalFormat, width, height, 0, sourceFormat, dataType, data);
}

void Texture::allocateCube(GLuint internalFormat, int width, GLuint sourceFormat, GLuint dataType, const void* data)
{
	for (int i = 0; i < 6; i++)
	{
		glTextureImage2DEXT
		(
			m_ID, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat,
			width, width, 0, sourceFormat, dataType, data
		);
	}
}
