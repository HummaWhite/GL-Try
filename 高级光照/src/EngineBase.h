#pragma once

#include <iostream>
#include <string>
#include <ctime>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "Model.h"
#include "Shader.h"
#include "Buffer.h"
#include "VertexArray.h"
#include "Renderer.h"
#include "Texture.h"
#include "Camera.h"
#include "Lighting.h"
#include "Shape.h"
#include "FPSTimer.h"
#include "VerticalSync.h"
#include "FrameBuffer.h"
#include "Skybox.h"
#include "CheckError.h"
#include "ShadowMap.h"
#include "Inputs.h"

class EngineBase
{
public:
	EngineBase(int width, int height);
	~EngineBase();

	int run();
	void setViewport(int x, int y, int width, int height);
	virtual void resizeWindow(int width, int height);

	int windowWidth() const { return m_WindowWidth; }
	int windowHeight() const { return m_WindowHeight; }

	GLFWwindow* window() const { return m_Window; }
	bool shouldTerminate() const { return m_ShouldTerminate; }
	void setTerminateStatus(bool status);

private:
	void setupGL(int width, int height);

	virtual void init() = 0;
	virtual void renderLoop() = 0;
	virtual void terminate() = 0;

	void error(const char* errString);

	virtual void processKey() = 0;
	virtual void processCursor() = 0;
	virtual void processScroll() = 0;

private:
	GLFWwindow* m_Window;
	bool m_ShouldTerminate;
	bool m_ErrorOccured;
	int m_WindowWidth, m_WindowHeight;
};