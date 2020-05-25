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

const int W_WIDTH = 1280;
const int W_HEIGHT = 720;
const int SHADOW_RES = 1024;

Camera camera(glm::vec3(0, 0, -3));
Light* spotLight = new Light(glm::vec3(0, 0, 0), glm::vec3(1.0f, 1.0f, 1.0f), VEC_UP, 10.0f, 12.5f);
int VERTICAL_SYNC = 1;
float GAMMA = 2.20f;
float EXPOSURE = 2.0f;
int SHADOW_ON = 1;

bool cursorDisabled = 1, F1Pressed = 0;
float lastCursorX = W_WIDTH / 2, lastCursorY = W_HEIGHT / 2;
bool first = 1;

Shader shader, scrShader, skyboxShader, lightShader, shadowShader;
Skybox skybox;

void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (!cursorDisabled) return;
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
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        camera.move(GLFW_KEY_Q);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera.move(GLFW_KEY_E);
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        camera.move(GLFW_KEY_R);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.move(GLFW_KEY_LEFT_SHIFT);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.move(GLFW_KEY_SPACE);
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
        spotLight->color = glm::vec3(1.0, 1.0, 1.0);
    else
        spotLight->color = glm::vec3(0.0f, 0.0f, 0.0f);
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

void setupShaders()
{
    lightShader.load("res/shader/light.shader");
    skyboxShader.load("res/shader/skyboxSphere.shader");
    shader.load("res/shader/PBR.shader");
    scrShader.load("res/shader/frameBuffer.shader");
    shadowShader.load("res/shader/shadow.shader");
}

void init()
{
    setupShaders();
    skybox.loadSphere("res/texture/017.hdr", GL_SRGB);
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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

    //glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);
    VerticalSyncStatus(VERTICAL_SYNC);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    init();

    std::vector<glm::vec3> bezierPoints =
    {
        { -2.0f, -2.0f,  1.0f }, { -0.15f, -2.1f, 0.0f }, { 1.5f, -2.4f, -2.2f },
        { -3.0f, -0.25f, 1.0f }, {  0.15f, -0.1f, 0.3f }, { 1.5f, -0.4f, 1.2f },
        { -2.5f, 1.75f, -2.4f }, { -0.05f,  1.9f, -1.5f }, { 1.5f, 2.4f, 1.2f }
    };

    Bezier bezier(2, 2, 20, 20, bezierPoints, Shape::VERTEX);
    bezier.addTangents();
    bezier.setupVA();
    Cube cube;
    cube.addTangents();
    cube.setupVA();
    Sphere sphere(40, 20, 1.0f, Shape::VERTEX);
    sphere.addTangents();
    sphere.setupVA();
    Square square;
    square.addTangents();
    square.setupVA();
    Torus torus(40, 20, 0.5f, 0.2f, Shape::VERTEX);
    torus.addTangents();
    torus.setupVA();
    BezierCurves teapot("res/model/teapotCGA.bpt", 20, 20, Shape::VERTEX);
    teapot.addTangents();
    teapot.setupVA();
    BezierCurves teaspoon("res/model/teaspoon.bpt", 20, 20, Shape::VERTEX);
    teaspoon.addTangents();
    teaspoon.setupVA();

    LightGroup lights;
    const int pointLightCount = 4;
    Light* light = new Light({ 0.0f, 0.0f, 4.0f }, { 1.0f, 1.0f, 1.0f });
    Light* light2 = new Light({ 6.0f, -6.0f, 0.0f }, { 1.0f, 0.0f, 1.0f });
    Light* light3 = new Light({ -4.0, -2.0f, -1.0f }, { 0.2f, 0.5f, 0.9f });
    int light0AttenLevel = -1;
    lights.push_back(light);
    lights.push_back(light2);
    lights.push_back(light3);
    lights.push_back(spotLight);

    FrameBuffer depthBuffer[pointLightCount];
    Texture depthBufferTex[pointLightCount];

    for (int i = 0; i < pointLightCount; i++)
    {
        depthBuffer[i].generate(SHADOW_RES, SHADOW_RES);
        depthBufferTex[i].generateDepthCube(SHADOW_RES);
        depthBuffer[i].attachTexture(GL_DEPTH_ATTACHMENT, depthBufferTex[i]);
        depthBuffer[i].activateAttachmentTargets({ GL_NONE });
    }

    Texture normMap;
    //normMap.loadSingle("res/texture/crafting_table_front_n.png");
    normMap.loadSingle("res/texture/diamond_ore_n.png");
    shader.setTexture("material.normalMap", normMap, normMap.slot);
    Texture ordTex;
    //ordTex.loadSingle("res/texture/crafting_table_front.png", GL_SRGB);
    ordTex.loadSingle("res/texture/diamond_ore.png", GL_SRGB);
    shader.setTexture("ordTex", ordTex, ordTex.slot);

    Buffer scrVb(SCREEN_COORD, 6, sizeof(SCREEN_COORD));
    BufferLayout scrLayout;
    scrLayout.add<float>(2);
    VertexArray scrVa;
    scrVa.addBuffer(scrVb, scrLayout);

    FrameBuffer scrFB;
    scrFB.generate(W_WIDTH, W_HEIGHT);
    RenderBuffer scrRB;
    scrRB.allocate(GL_DEPTH24_STENCIL8, W_WIDTH, W_HEIGHT);
    Texture FBTex;
    FBTex.generate2D(W_WIDTH, W_HEIGHT, GL_RGB16F);
    scrFB.attachRenderBuffer(GL_DEPTH_STENCIL_ATTACHMENT, scrRB);
    scrFB.attachTexture(GL_COLOR_ATTACHMENT0, FBTex);

    glm::mat4 model(1.0f);
    glm::mat4 proj;

    glm::vec3 cubePositions[] =
    {
        { -2.0f,  1.0f,  2.5f },
        { 2.0f,  10.0f, 0.0f },
        { -1.5f, -2.2f, -2.5f },
        { -3.8f, 4.0f, -2.3f },
        { 2.4f, -0.4f, -3.5f },
        { -1.7f,  3.0f, 7.5f },
        { 1.3f, -2.0f, 2.5f },
        { 1.5f,  2.0f, 1.5f },
        { 1.5f,  0.2f, 6.5f },
        { -1.3f,  1.0f, -1.5f }
    };

    int objectCount = 4;

    glm::vec3 matAlbedo(1.0f, 1.0f, 1.0f);
    float matMetallic(0.75f);
    float matRoughness(0.2f);
    float matAo(0.1f);

    int useTexture = 0;
    int useNormalMap = 1;
    int useReflMap = 1;
    float reflStrength = 0.03f;
    int vaIndex = 0;

    VertexArray* va[] = { &cube.VA(), &sphere.VA(), &square.VA(), &bezier.VA(), &torus.VA(), &teapot.VA(), &teaspoon.VA() };

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 430");

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        renderer.clear(0.0f, 0.0f, 0.0f);

        float timeValue = glfwGetTime();
        proj = glm::perspective(glm::radians(camera.FOV()), (float)W_WIDTH / (float)W_HEIGHT, 0.1f, 100.0f);
        light->setAttenuationLevel(light0AttenLevel);
        //light->pos.y = sin(timeValue * 3.0f) * 6.0f;
        //light->pos.x = cos(timeValue * 3.0f) * 6.0f;
        //light->pos.z = sin(timeValue * 0.7f) * 4.0f;

        float aspect = 1.0f;
        float shadowNear = 0.1f;
        float shadowFar = 50.0f;
        glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, shadowNear, shadowFar);
        glViewport(0, 0, SHADOW_RES, SHADOW_RES);
        shadowShader.enable();

        for (int i = 0; i < pointLightCount; i++)
        {
            std::vector<glm::mat4> shadowTransforms;
            glm::vec3 lightPos = lights[i]->pos;
            shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
            shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
            shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
            shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
            shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
            shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));

            renderer.clearFrameBuffer(depthBuffer[i], 0.0f, 0.0f, 0.0f);
            for (int i = 0; i < 6; i++)
                shadowShader.setUniformMat4(("shadowMatrices[" + std::to_string(i) + "]").c_str(), shadowTransforms[i]);
            shadowShader.setUniform1f("farPlane", shadowFar);
            shadowShader.setUniformVec3("lightPos", lightPos);
            for (int i = 0; i < objectCount; i++)
            {
                model = glm::mat4(1.0f);
                float scale = (float)i / 5 + 1;
                model = glm::translate(model, cubePositions[i]);
                model = glm::scale(model, glm::vec3(scale, scale, scale));
                model = glm::rotate(model, glm::radians(timeValue * 30.0f * i + 20.0f * i), glm::vec3(1.0f, 0.3f, 0.5f));
                shadowShader.setUniformMat4("model", model);
                renderer.drawToFrameBuffer(depthBuffer[i], *va[vaIndex], shadowShader);
            }

        }

        glViewport(0, 0, W_WIDTH, W_HEIGHT);
        renderer.clearFrameBuffer(scrFB);
        //light->attenuation.x = light->attenuation.y = 0;
        //light2->attenuation.x = light2->attenuation.y = 0;
        //light3->attenuation.x = light3->attenuation.y = 0;

        shader.enable();
        shader.setUniformMat4("proj", proj);
        shader.setUniformMat4("view", camera.getViewMatrix());
        shader.setUniformVec3("viewPos", camera.pos());
        shader.setUniform1f("normDir", 1.0);
        shader.setLight(lights);

        shader.setTexture("ordTex", ordTex, ordTex.slot);
        shader.setMaterial(matAlbedo, matMetallic, matRoughness, matAo);
        shader.setUniform1i("useTexture", useTexture);
        shader.setUniform1i("useNormalMap", useNormalMap);
        shader.setTexture("material.normalMap", normMap, normMap.slot);
        shader.setTexture("material.reflMap", skybox.texture(), skybox.texture().slot);
        shader.setUniform1i("useReflMap", useReflMap);
        shader.setUniform1f("material.reflStrength", reflStrength);
        shader.setUniform1i("shadowOn", SHADOW_ON);
        for (int i = 0; i < pointLightCount; i++)
        {
            depthBufferTex[i].bind();
            shader.setUniform1i(("depthMapPoint[" + std::to_string(i) + "]").c_str(), depthBufferTex[i].slot);
        }
        shader.setUniform1f("farPlane", shadowFar);

        for (int i = 0; i < objectCount; i++)
        {
            model = glm::mat4(1.0f);
            float scale = (float)i / 5 + 1;
            model = glm::translate(model, cubePositions[i]);
            model = glm::scale(model, glm::vec3(scale, scale, scale));
            model = glm::rotate(model, glm::radians(timeValue * 30.0f * i + 20.0f * i), glm::vec3(1.0f, 0.3f, 0.5f));
            shader.useModelMatrix(model);
            renderer.drawToFrameBuffer(scrFB, *va[vaIndex], shader);
        }

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0, 0.0, -5.0f));
        model = glm::scale(model, glm::vec3(40.0f, 40.0f, 40.0f));
        glm::vec3 wallColor(1.0f, 1.0f, 1.0f);
        shader.useModelMatrix(model);
        shader.setMaterial(wallColor, 0.0f, 1.0f, 0.0f);
        renderer.drawToFrameBuffer(scrFB, square.VA(), shader);

        lightShader.enable();
        lightShader.setUniformMat4("proj", proj);
        lightShader.setUniformMat4("view", camera.getViewMatrix());
        spotLight->pos = camera.pos();
        spotLight->dir = camera.pointing();
        for (int i = 0; i < lights.size(); i++)
        {
            if (lights[i] == spotLight) continue;
            model = glm::translate(glm::mat4(1.0f), lights[i]->pos);
            lightShader.setUniformMat4("model", model);
            lightShader.setUniformVec3("lightColor", glm::length(lights[i]->color) * glm::normalize(lights[i]->color));
            renderer.drawToFrameBuffer(scrFB, sphere.VA(), lightShader);
        }

        scrFB.bind();
        skybox.setProjection(proj);
        skybox.setView(glm::mat4(glm::mat3(camera.getViewMatrix())));
        skybox.draw();
        scrFB.unbind();

        renderer.clear();
        scrShader.enable();
        scrShader.setTexture("frameBuffer", FBTex, FBTex.slot);
        scrShader.setUniform1f("gamma", GAMMA);
        scrShader.setUniform1f("exposure", EXPOSURE);
        renderer.draw(scrVa, scrShader);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        {
            ImGui::Begin("TEST");
            {
                ImGui::Text("Light0");
                ImGui::ColorEdit3("Color0", (float*)&light->color);
                ImGui::SliderFloat3("Pos0", (float*)&light->pos, -20.0f, 20.0f);
                ImGui::SliderInt("Attenuation", &light0AttenLevel, -1, 11);
                ImGui::SliderFloat("Strength", &light->strength, 0.1f, 100.0f);
                ImGui::Text("Light1");
                ImGui::ColorEdit3("Color1", (float*)&light2->color);
                ImGui::SliderFloat3("Pos1", (float*)&light2->pos, -20.0f, 20.0f);
                ImGui::Text("Light2");
                ImGui::ColorEdit3("Color2", (float*)&light3->color);
                ImGui::SliderFloat3("Pos2", (float*)&light3->pos, -20.0f, 20.0f);
                ImGui::SliderInt("Vertical Sync", &VERTICAL_SYNC, 0, 1);
                ImGui::ColorEdit3("Albedo", (float*)&matAlbedo);
                ImGui::SliderFloat("Metallic", &matMetallic, 0.0f, 1.0f);
                ImGui::SliderFloat("Roughness", &matRoughness, 0.0f, 1.0f);
                ImGui::SliderFloat("Ao", &matAo, 0.0f, 1.0f);
                ImGui::SliderInt("Shadow On", &SHADOW_ON, 0, 1);
                ImGui::SliderInt("Texture on", &useTexture, 0, 1);
                ImGui::SliderInt("NormalMap on", &useNormalMap, 0, 1);
                ImGui::SliderInt("ReflMap on", &useReflMap, 0, 1);
                ImGui::SliderFloat("ReflStrength", &reflStrength, 0.0f, 1.0f);
                ImGui::SliderFloat("Gamma", &GAMMA, 1.0f, 4.0f);
                ImGui::SliderFloat("Exposure", &EXPOSURE, 0.01f, 20.0f);
                ImGui::SliderInt("Shape", &vaIndex, 0, 6);
                ImGui::Text("x: %.3f y: %.3f z: %.3f  FOV: %.1f", camera.pos().x, camera.pos().y, camera.pos().z, camera.FOV());
                ImGui::Text("Render Time: %.3f ms, FPS: %.3f", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
                ImGui::Text("\n");
                ImGui::Text("WASD / Lshift / Space  - move");
                ImGui::Text("Mouse                  - view");
                ImGui::Text("F1                     - release mouse");
                ImGui::Text("ESC                    - exit");
            }
            ImGui::End();
        }
        ImGui::EndFrame();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        VerticalSyncStatus(VERTICAL_SYNC);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}