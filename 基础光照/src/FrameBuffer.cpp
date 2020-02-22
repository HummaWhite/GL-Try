#include "FrameBuffer.h"

FrameBuffer::FrameBuffer()
{
	glGenFramebuffers(1, &ID);
	//glBindFramebuffer(GL_FRAMEBUFFER, ID);
}

FrameBuffer::~FrameBuffer()
{
	glDeleteFramebuffers(1, &ID);
}

void FrameBuffer::bind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, ID);
}

void FrameBuffer::unbind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
