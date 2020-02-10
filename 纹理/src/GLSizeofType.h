#pragma once

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

static GLuint sizeofGLType(GLuint type)
{
	switch (type)
	{
	case GL_FLOAT:			return sizeof(GLfloat);
	case GL_INT:			return sizeof(GLint);
	case GL_UNSIGNED_INT:	return sizeof(GLuint);
	case GL_UNSIGNED_BYTE:	return sizeof(GLubyte);
	case GL_BYTE:			return sizeof(GLbyte);
	}
	return 0;
}