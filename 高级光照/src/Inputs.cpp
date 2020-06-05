#include "Inputs.h"

namespace Inputs
{
    void windowSizeCallback(GLFWwindow* window, int width, int height)
    {
        curWindowWidth = width;
        curWindowHeight = height;
    }

    void cursorPosCallback(GLFWwindow* window, double xpos, double ypos)
    {
        if (!cursorDisabled) return;
        if (firstCursorMove == 1)
        {
            lastCursorX = xpos;
            lastCursorY = ypos;
            firstCursorMove = 0;
            return;
        }
        lastCursorX = xpos;
        lastCursorY = ypos;
        cursorX = xpos;
        cursorY = ypos;
    }

    void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
    {
        scrollOffset = yoffset;
    }

    void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
    {
        if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS)
        {
            F1Pressed = 1;
        }
        if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_RELEASE)
        {
            if (F1Pressed)
            {
                if (cursorDisabled)
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                else
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                cursorDisabled ^= 1;
                F1Pressed = 0;
            }
        }
    }
}
