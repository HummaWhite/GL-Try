#include <iostream>
#include <ctime>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Renderer.h"
#include "Texture.h"
#include "Camera.h"
#include "Lighting.h"

const int W_WIDTH = 1280;
const int W_HEIGHT = 720;

static Camera camera(glm::vec3(0, 0, -3));

struct FPSDetector
{
    FPSDetector() : lastT(0) {}
    void work()
    {
        clock_t intv = clock() - lastT;
        if (intv != 0)
            std::cout << (float)intv / CLOCKS_PER_SEC << " " << (float)CLOCKS_PER_SEC / intv << "\n";
        lastT = clock();
    }
    clock_t lastT;
} FPS;

void initGLFWdata();
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

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

float lastCursorX = W_WIDTH / 2, lastCursorY = W_HEIGHT / 2;
bool first = 1;

void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (first == 1)
    {
        lastCursorX = xpos;
        lastCursorY = ypos;
        first = 0;
        return;
    }
    float offsetX = (xpos - lastCursorX) * CAMERA_ROTATE_SENSITIVITY;
    float offsetY = (ypos - lastCursorY) * CAMERA_ROTATE_SENSITIVITY;
    glm::vec3 offset = glm::vec3(-offsetX, -offsetY, 0);
    camera.rotate(offset);
    lastCursorX = xpos;
    lastCursorY = ypos;
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.changeFOV(yoffset);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.move(GLFW_KEY_W);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.move(GLFW_KEY_S);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.move(GLFW_KEY_A);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.move(GLFW_KEY_D);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.move(GLFW_KEY_LEFT_SHIFT);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.move(GLFW_KEY_SPACE);
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
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);

    float vertices[] =
    {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f, 0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  0.0f, 0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f, 0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f, 0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f, 0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f, 0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f, 0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, 0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f, 0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f, 0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  0.0f, 0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f, 0.0f,  1.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  -1.0f, 0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  -1.0f, 0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  -1.0f, 0.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,   1.0f, 0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,   1.0f, 0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,   1.0f, 0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,   1.0f, 0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f, -1.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,  0.0f, -1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, -1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, -1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f, -1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f, -1.0f, 0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  1.0f, 0.0f
    };


    GLuint indices[] =
    {
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 5, 6, 7,
        3, 0, 4, 0, 4, 7,
        2, 1, 5, 1, 5, 6,
        0, 1, 5, 1, 5, 4,
        3, 2, 6, 2, 6, 7
    };

    VertexBuffer vb(vertices, 36, sizeof(vertices));
    VertexBufferLayout layout;
    layout.add<float>(3);
    layout.add<float>(2);
    layout.add<float>(3);
    VertexArray va;
    va.addBuffer(vb, layout);
    IndexBuffer eb(indices, 36);
    Shader shader("res/shader/basic.shader");
    Texture texture("res/texture/tex.png");
    texture.bind();
    //shader.setUniform1i("u_Texture", 0);

    VertexArray lightVa;
    lightVa.addBuffer(vb, layout);
    Shader lightShader("res/shader/light.shader");
    LightGroup lights;
    Light* light = new Light(Light::POINT, glm::vec3(0, 0, 4.0f), glm::vec3(1.0f, 1.0f, 1.0f));
    Light* light2 = new Light(Light::POINT, glm::vec3(6.0f, -6.0f, 0), glm::vec3(1.0f, 0.0f, 1.0f));
    Light* light3 = new Light(Light::POINT, glm::vec3(-4.0, -2.0f, -1.0f), glm::vec3(0.2f, 0.5f, 0.9f));
    Light* light4 = new Light(Light::SPOT, glm::vec3(0, 0, 0), VEC_UP, glm::vec3(0.0f, 1.0f, 0.0f), 10.0f, 15.0f);
    lights.add(light);
    lights.add(light2);
    lights.add(light3);
    lights.add(light4);

    Renderer renderer;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);

    glm::mat4 model(1.0f);
    glm::mat4 view(1.0f);
    view = glm::translate(view, glm::vec3(0.0f, -3.0f, 0.0f));
    glm::mat4 proj;

    glm::vec3 cubePositions[] =
    {
        glm::vec3(-2.0f,  1.0f,  2.5f),
        glm::vec3(2.0f,  10.0f, 0.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, 4.0f, -2.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, 7.5f),
        glm::vec3(1.3f, -2.0f, 2.5f),
        glm::vec3(1.5f,  2.0f, 1.5f),
        glm::vec3(1.5f,  0.2f, 6.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };

    glm::vec3 objectColor = glm::vec3(1.0f, 0.5f, 0.0f);

    while (!glfwWindowShouldClose(window))
    {
        //FPS.work();
        processInput(window);

        //renderer.clear(0.3f, 0.2f, 0.3f, 1.0f);
        renderer.clear(0.0f, 0.0f, 0.0f);
        
        float timeValue = glfwGetTime();
        float blueValue = sin(timeValue * 1.4f) / 2.0f + 0.5f;
        float additionY = sin(timeValue * 3.0f) * 6.0f;
        proj = glm::perspective(glm::radians(camera.FOV()), (float)W_WIDTH / (float)W_HEIGHT, 0.1f, 100.0f);

        shader.enable();

        //light->pos.y = additionY;
        //shader.setUniform4f("vertexColor", 1.0f, 0.5f, blueValue, 1.0f);
        shader.setUniformMat4("proj", proj);
        shader.setUniformMat4("view", camera.getViewMatrix());
        shader.setUniformVec3("viewPos", camera.pos());
        shader.setLight(lights);
        shader.setUniformVec3("material.ambient", objectColor * 0.1f);
        shader.setUniformVec3("material.diffuse", objectColor);
        shader.setUniformVec3("material.specular", objectColor);
        shader.setUniform1f("material.shininess", 32.0f);

        for (int i = 0; i < 10; i++)
        {
            model = glm::mat4(1.0f);
            //model = glm::rotate(glm::mat4(1.0f), glm::radians(-55.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            float scale = (float)i / 5 + 1;
            model = glm::translate(model, cubePositions[i]);
            model = glm::scale(model, glm::vec3(scale, scale, scale));
            model = glm::rotate(model, glm::radians(timeValue * 30.0f * i + 20.0f * i), glm::vec3(1.0f, 0.3f, 0.5f));
            shader.setUniformMat4("model", model);
            glm::mat3 modelInv = glm::mat3(glm::transpose(glm::inverse(model)));
            shader.setUniformMat3("modelInv", modelInv);
            renderer.draw(va, shader);
        }

        lightShader.enable();
        lightShader.setUniformMat4("proj", proj);
        lightShader.setUniformMat4("view", camera.getViewMatrix());
        light4->pos = camera.pos();
        light4->dir = camera.pointing();
        for (int i = 0; i < lights.count(); i++)
        {
            if (lights[i]->type != Light::POINT) continue;
            model = glm::translate(glm::mat4(1.0f), lights[i]->pos);
            lightShader.setUniformMat4("model", model);
            lightShader.setUniformVec3("lightColor", lights[i]->color);
            renderer.draw(lightVa, lightShader);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}