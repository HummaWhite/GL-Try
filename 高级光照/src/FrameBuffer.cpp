#include "FrameBuffer.h"

FrameBuffer::~FrameBuffer()
{
	glDeleteFramebuffers(1, &fbID);
	glDeleteRenderbuffers(1, &rbID);
}

void FrameBuffer::generate()
{
	glGenFramebuffers(1, &fbID);
	glGenRenderbuffers(1, &rbID);
}

void FrameBuffer::bind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, fbID);
	glBindRenderbuffer(GL_RENDERBUFFER, rbID);
}

void FrameBuffer::unbind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void FrameBuffer::attachRenderBuffer() const
{
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbID);
}