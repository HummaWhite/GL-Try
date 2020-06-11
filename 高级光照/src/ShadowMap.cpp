#include "ShadowMap.h"

void ShadowMap::init(int type, int width, GLuint format)
{
	if (type == DIRECTIONAL)
	{
		m_FB.generate(width, width);
		m_Tex.generateDepth2D(width, width, format);
	}
	else
	{
		m_FB.generate(width, width);
		m_Tex.generateDepthCube(width);
	}
	m_FB.attachTexture(GL_DEPTH_ATTACHMENT, m_Tex);
	m_FB.activateAttachmentTargets({ GL_NONE });
	m_Size = width;
}

void ShadowMap::bind()
{
	m_FB.bind();
}

void ShadowMap::unbind()
{
	m_FB.unbind();
}

void ShadowMap::linkTextureUnit(int unit)
{
	m_Tex.bind(unit);
}
