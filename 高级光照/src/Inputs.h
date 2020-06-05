#pragma once

#include <iostream>
#include <string>
#include <ctime>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Inputs
{
	void windowSizeCallback(GLFWwindow* window, int width, int height);
	void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
	void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
	void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);

	static int curWindowWidth = 1280, curWindowHeight = 720;
	static bool cursorDisabled = true;
	static float cursorX, cursorY;
	static float lastCursorX, lastCursorY;
	static float scrollOffset = 0;
	static bool firstCursorMove = true;
	static bool F1Pressed = false;
}