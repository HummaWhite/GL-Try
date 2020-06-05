#pragma once

#include "FrameBuffer.h"
#include "Texture.h"
#include "RenderBuffer.h"

class ShadowMap
{
public:
	void init(int type, int width);

	void bind();
	void unbind();

	void linkTextureUnit(int unit);

	enum ShadowMapType
	{
		DIRECTIONAL = 0,
		POINT
	};

private:
	FrameBuffer m_FB;
	Texture m_Tex;
};