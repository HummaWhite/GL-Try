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

/*void Texture::attachDepthBufferCube(const FrameBuffer& depthBuffer, int resolution)
{
	if (m_Loaded)
	{
		std::cout << "Error: texture already loaded for this object" << std::endl;
		return;
	}
	m_TextureType = GL_TEXTURE_CUBE_MAP;
	glBindTexture(m_TextureType, m_ID);

	for (int i = 0; i < 6; i++)
	{
		glTexImage2D
		(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
			resolution, resolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr
		);
	}

	glTextureParameteri(m_ID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(m_ID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureParameteri(m_ID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(m_ID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureParameteri(m_ID, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	depthBuffer.bind();
	//glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_ID, 0);
	glNamedFramebufferTexture(depthBuffer.frameBufferID(), GL_DEPTH_ATTACHMENT, m_ID, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	depthBuffer.unbind();
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Error: framebuffer not complete" << std::endl;

	glBindTexture(m_TextureType, 0);
	m_Loaded = true;
	slot = m_SlotsUsed++;
}

void Texture::attachColorBufferCube(const FrameBuffer& frameBuffer, int resolution, GLuint colorFormat)
{
	if (m_Loaded)
	{
		std::cout << "Error: texture already loaded for this object" << std::endl;
		return;
	}
	m_TextureType = GL_TEXTURE_CUBE_MAP;
	glBindTexture(m_TextureType, m_ID);

	for (int i = 0; i < 6; i++)
	{
		glTexImage2D
		(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, colorFormat,
			resolution, resolution, 0, GL_RGB, GL_FLOAT, nullptr
		);
	}

	glTextureParameteri(m_ID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(m_ID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureParameteri(m_ID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(m_ID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureParameteri(m_ID, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	frameBuffer.bind();
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH32F_STENCIL8, resolution, resolution);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_TextureType, m_ID, 0);
	frameBuffer.attachRenderBuffer();
	frameBuffer.unbind();

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
	glBindTexture(m_TextureType, m_ID);

	glTexImage2D(m_TextureType, 0, type, width, height, 0, type, GL_FLOAT, nullptr);

	glTexParameteri(m_TextureType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(m_TextureType, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	frameBuffer.bind();
	glFramebufferTexture2D(GL_FRAMEBUFFER, type, m_TextureType, m_ID, 0);
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
	glBindTexture(m_TextureType, m_ID);

	glTexImage2D(m_TextureType, 0, colorFormat, width, height, 0, GL_RGB, GL_FLOAT, nullptr);

	glTexParameteri(m_TextureType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(m_TextureType, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	frameBuffer.bind();
	glNamedRenderbufferStorage(frameBuffer.renderBufferID(), GL_DEPTH24_STENCIL8, width, height);
	glNamedFramebufferTexture2DEXT(frameBuffer.frameBufferID(), GL_COLOR_ATTACHMENT0, m_TextureType, m_ID, 0);
	frameBuffer.attachRenderBuffer();

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Error: framebuffer not complete" << std::endl;

	glBindTexture(m_TextureType, 0);
	m_Loaded = true;
	slot = m_SlotsUsed++;
}*/

void Texture::bind() const
{
	//glBindTextureUnit(GL_TEXTURE0 + this->slot, m_ID);
	glActiveTexture(GL_TEXTURE0 + this->slot);
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
