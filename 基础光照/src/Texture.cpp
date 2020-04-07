#include "Texture.h"
#include "stb_image/stb_image.h"

int Texture::m_SlotsUsed = 0;

Texture::Texture() :
	m_Width(0), m_Height(0), m_BitsPerPixel(0),
	m_TextureType(0), m_Loaded(false)
{
	glGenTextures(1, &ID);
}

Texture::~Texture()
{
	glDeleteTextures(1, &ID);
}

void Texture::loadSingle(const std::string& filePath, GLuint internalType, GLuint filterType)
{
	if (m_Loaded)
	{
		std::cout << "Error: texture already loaded for this object" << std::endl;
		return;
	}
	m_TextureType = GL_TEXTURE_2D;
	glBindTexture(m_TextureType, ID);

	//stbi_set_flip_vertically_on_load(1);
	GLubyte* data = stbi_load(filePath.c_str(), &m_Width, &m_Height, &m_BitsPerPixel, 4);
	if (data == nullptr)
	{
		std::cout << "Error: unable to load texture: " << filePath << std::endl;
		return;
	}

	glTexImage2D(m_TextureType, 0, internalType, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glTexParameteri(m_TextureType, GL_TEXTURE_MIN_FILTER, filterType);
	glTexParameteri(m_TextureType, GL_TEXTURE_MAG_FILTER, filterType);
	glTexParameteri(m_TextureType, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(m_TextureType, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(m_TextureType, 0);

	if (data != nullptr) stbi_image_free(data);
	m_Loaded = true;
	slot = m_SlotsUsed++;
}

void Texture::loadCube(const std::vector<std::string>& filePaths, GLuint internalType, GLuint filterType)
{
	if (m_Loaded)
	{
		std::cout << "Error: texture already loaded for this object" << std::endl;
		return;
	}
	m_TextureType = GL_TEXTURE_CUBE_MAP;
	glBindTexture(m_TextureType, ID);

	//stbi_set_flip_vertically_on_load(1);

	for (int i = 0; i < filePaths.size(); i++)
	{
		GLubyte* data = stbi_load(filePaths[i].c_str(), &m_Width, &m_Height, &m_BitsPerPixel, 4);
		if (data == nullptr)
		{
			std::cout << "Error: unable to load texture: " << filePaths[i] << std::endl;
			return;
		}
		glTexImage2D
		(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
			0, internalType, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data
		);
		stbi_image_free(data);
	}

	glTexParameteri(m_TextureType, GL_TEXTURE_MAG_FILTER, filterType);
	glTexParameteri(m_TextureType, GL_TEXTURE_MIN_FILTER, filterType);
	glTexParameteri(m_TextureType, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(m_TextureType, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(m_TextureType, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(m_TextureType, 0);

	m_Loaded = true;
	slot = m_SlotsUsed++;
}

void Texture::attachDepthBufferCube(const FrameBuffer& depthBuffer, int resolution)
{
	if (m_Loaded)
	{
		std::cout << "Error: texture already loaded for this object" << std::endl;
		return;
	}
	m_TextureType = GL_TEXTURE_CUBE_MAP;
	glBindTexture(m_TextureType, ID);

	for (int i = 0; i < 6; i++)
	{
		glTexImage2D
		(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
			resolution, resolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr
		);
	}

	glTexParameteri(m_TextureType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(m_TextureType, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(m_TextureType, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(m_TextureType, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(m_TextureType, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	depthBuffer.bind();
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, ID, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	depthBuffer.unbind();
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Error: framebuffer not complete" << std::endl;

	glBindTexture(m_TextureType, 0);
	m_Loaded = true;
	slot = m_SlotsUsed++;
}

void Texture::attachFrameBuffer2D(const FrameBuffer& frameBuffer, GLuint type, int width, int height)
{
	if (m_Loaded)
	{
		std::cout << "Error: texture already loaded for this object" << std::endl;
		return;
	}
	m_TextureType = GL_TEXTURE_2D;
	glBindTexture(m_TextureType, ID);

	glTexImage2D(m_TextureType, 0, type, width, height, 0, type, GL_FLOAT, nullptr);

	glTexParameteri(m_TextureType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(m_TextureType, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	frameBuffer.bind();
	glFramebufferTexture2D(GL_FRAMEBUFFER, type, m_TextureType, ID, 0);
	frameBuffer.unbind();

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Error: framebuffer not complete" << std::endl;

	glBindTexture(m_TextureType, 0);
	m_Loaded = true;
	slot = m_SlotsUsed++;
}

void Texture::attachColorBuffer2D(const FrameBuffer& frameBuffer, int width, int height, GLuint colorFormat)
{
	if (m_Loaded)
	{
		std::cout << "Error: texture already loaded for this object" << std::endl;
		return;
	}
	m_TextureType = GL_TEXTURE_2D;
	glBindTexture(m_TextureType, ID);

	glTexImage2D(m_TextureType, 0, colorFormat, width, height, 0, GL_RGB, GL_FLOAT, nullptr);

	glTexParameteri(m_TextureType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(m_TextureType, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	frameBuffer.bind();
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH32F_STENCIL8, width, height);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_TextureType, ID, 0);
	frameBuffer.attachRenderBuffer();
	//frameBuffer.unbind();

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Error: framebuffer not complete" << std::endl;

	glBindTexture(m_TextureType, 0);
	m_Loaded = true;
	slot = m_SlotsUsed++;
}

void Texture::bind() const
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(m_TextureType, ID);
}

void Texture::bind(int slot) const
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(m_TextureType, ID);
}

void Texture::unbind() const
{
	glBindTexture(m_TextureType, 0);
}