#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Shader.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"

const int W_WIDTH = 1280;
const int W_HEIGHT = 720;

void initGLFWdata();
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void VAOSettings();

void initGLFWdata()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main()
{
    initGLFWdata();

    GLFWwindow* window = glfwCreateWindow(W_WIDTH, W_HEIGHT, "OpenGL-Try", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, W_WIDTH, W_HEIGHT);

    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    float vertices[] =
    {
         0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
         0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f,  0.0f, 0.0f, -1.0f,
        -0.5f,  0.5f,  0.0f, 0.0f, -1.0f
    };

    GLuint indices[] = {0, 1, 2, 2, 3, 0};

    VertexBuffer vb(vertices, sizeof(vertices));
    VertexBufferLayout layout;
    layout.add<float>(2);
    layout.add<GL_FLOAT>(3);
    VertexArray va;
    va.addBuffer(vb, layout);
    IndexBuffer eb(indices, sizeof(indices));
    Shader shader("res/shader/basic.shader");

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.3f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.enable();
        
        float timeValue = glfwGetTime();
        float blueValue = sin(timeValue * 1.4f) / 2.0f + 0.5f;

        shader.setUniform4f("vertexColor", 1.0f, 0.5f, blueValue, 1.0f);
        shader.setUniform1f("additionY", sin(timeValue * 3.0f) / 2.0f);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}