#include "Engine.h"

Engine::Engine(int width, int height) :
    EngineBase(width, height),
    camera({ 0, -20, 8 }),
    verticalSync(true),
    gamma(2.2f),
    exposure(2.0f),
    shadowOn(true),
    useTexture(0),
    useNormalMap(0),
    useReflMap(0),
    reflStrength(0.03f),
    screenFB(nullptr),
    screenRB(nullptr),
    screenFBTex(nullptr),
    matAlbedo(1.0f, 1.0f, 1.0f),
    matMetallic(0.75f),
    matRoughness(0.2f),
    matAo(0.1f)
{
}

Engine::~Engine()
{
}

void Engine::init()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    this->setupFrameBuffersAndTextures();
    this->setupLights();
    this->setupObjects();
    this->setupShaders();
    this->setupGUI();

    skybox.loadSphere("res/texture/017.hdr", GL_SRGB);
}

void Engine::renderLoop()
{
    //this->resizeWindow();
    this->processCursor();
    this->processScroll();
    this->processKey();

    renderer.clear(0.0f, 0.0f, 0.0f);

    this->setViewport(0, 0, SHADOW_RES, SHADOW_RES);
    if (shadowOn) this->shadowPass(shadowMapPoint, lights, objects);

    this->setViewport(0, 0, this->windowWidth(), this->windowHeight());
    this->ZPass(ZBuffer, objects);

    this->renderPass();

    this->postPass();

    this->renderGUI();

    VerticalSyncStatus(verticalSync);

    glfwSwapBuffers(this->window());
    glfwPollEvents();
}

void Engine::terminate()
{
    if (screenFB != nullptr)
    {
        delete screenFB;
        delete screenRB;
        delete screenFBTex;
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void Engine::resizeWindow()
{
    int width = Inputs::curWindowWidth;
    int height = Inputs::curWindowHeight;
    std::cout << width << " " << height << std::endl;
    this->EngineBase::resizeWindow(width, height);
    this->resetScreenBuffer(width, height);
}

void Engine::resetScreenBuffer(int width, int height)
{
    if (screenFB != nullptr)
    {
        delete screenFB;
        delete screenRB;
        delete screenFBTex;
    }
    screenFB = new FrameBuffer;
    screenRB = new RenderBuffer;
    screenFBTex = new Texture;

    screenFB->generate(this->windowWidth(), this->windowHeight());
    screenRB->allocate(GL_DEPTH24_STENCIL8, this->windowWidth(), this->windowHeight());
    screenFBTex->generate2D(this->windowWidth(), this->windowHeight(), GL_RGB16F);
    screenFB->attachRenderBuffer(GL_DEPTH_STENCIL_ATTACHMENT, *screenRB);
    screenFB->attachTexture(GL_COLOR_ATTACHMENT0, *screenFBTex);
}

void Engine::processKey()
{
    if (glfwGetKey(this->window(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(this->window(), true);
        this->setTerminateStatus(true);
    }
    if (glfwGetKey(this->window(), GLFW_KEY_W) == GLFW_PRESS)
        camera.move(GLFW_KEY_W);
    if (glfwGetKey(this->window(), GLFW_KEY_S) == GLFW_PRESS)
        camera.move(GLFW_KEY_S);
    if (glfwGetKey(this->window(), GLFW_KEY_A) == GLFW_PRESS)
        camera.move(GLFW_KEY_A);
    if (glfwGetKey(this->window(), GLFW_KEY_D) == GLFW_PRESS)
        camera.move(GLFW_KEY_D);
    if (glfwGetKey(this->window(), GLFW_KEY_Q) == GLFW_PRESS)
        camera.move(GLFW_KEY_Q);
    if (glfwGetKey(this->window(), GLFW_KEY_E) == GLFW_PRESS)
        camera.move(GLFW_KEY_E);
    if (glfwGetKey(this->window(), GLFW_KEY_R) == GLFW_PRESS)
        camera.move(GLFW_KEY_R);
    if (glfwGetKey(this->window(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.move(GLFW_KEY_LEFT_SHIFT);
    if (glfwGetKey(this->window(), GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.move(GLFW_KEY_SPACE);
}

void Engine::processCursor()
{
    float offsetX = (Inputs::cursorX - Inputs::lastCursorX) * CAMERA_ROTATE_SENSITIVITY;
    float offsetY = (Inputs::cursorY - Inputs::lastCursorY) * CAMERA_ROTATE_SENSITIVITY;
    glm::vec3 offset = glm::vec3(-offsetX, -offsetY, 0);
    camera.rotate(offset);
}

void Engine::processScroll()
{
    camera.changeFOV(Inputs::scrollOffset);
}

void Engine::setupFrameBuffersAndTextures()
{
    for (int i = 0; i < LIGHT_COUNT; i++)
    {
        shadowMapPoint[i].init(ShadowMap::POINT, SHADOW_RES);
    }

    resetScreenBuffer(this->windowWidth(), this->windowHeight());
}

void Engine::setupLights()
{
    Light* light = new Light({ 0.0f, 0.0f, 14.0f }, { 1.0f, 1.0f, 1.0f });
    Light* light2 = new Light({ 6.0f, -6.0f, 0.0f }, { 1.0f, 0.0f, 1.0f });
    Light* light3 = new Light({ -4.0, -2.0f, -1.0f }, { 0.2f, 0.5f, 0.9f });
    lights.push_back(light);
    lights.push_back(light2);
    lights.push_back(light3);
}

void Engine::setupObjects()
{
    screenVB.allocate(sizeof(SCREEN_COORD), SCREEN_COORD, 6);
    screenVA.addBuffer(screenVB, LAYOUT_POS2);

    bunny.loadModel("res/model/bunny.obj");
    bunny.setPos(glm::vec3(0.0f, 0.0f, 9.0f));
    objects.push_back(&bunny);

    nano.loadModel("res/model/nanosuit/nanosuit.obj");
    nano.setPos(glm::vec3(2.0f, 0.0f, -4.0f));
    objects.push_back(&nano);

    square = new Square();
    square->addTangents();
    square->setupVA();
    sphere = new Sphere(20, 10, 1.0f, Shape::VERTEX);
    sphere->addTangents();
    sphere->setupVA();
}

void Engine::setupShaders()
{
    lightShader.load("res/shader/light.shader");
    skyboxShader.load("res/shader/skyboxSphere.shader");
    shader.load("res/shader/PBR.shader");
    scrShader.load("res/shader/frameBuffer.shader");
    shadowShader.load("res/shader/shadow.shader");
    zShader.load("res/shader/Zpass.shader");
}

void Engine::setupGUI()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsClassic();
    ImGui_ImplGlfw_InitForOpenGL(this->window(), true);
    ImGui_ImplOpenGL3_Init("#version 450");
}

void Engine::ZPass(FrameBuffer& zBuffer, std::vector<Model*> objects)
{
    glm::mat4 proj = glm::perspective(glm::radians(camera.FOV()), (float)this->windowWidth() / (float)this->windowHeight(), 0.1f, 100.0f);
    glm::mat4 view = camera.getViewMatrix();

    zShader.setUniformMat4("VPmatrix", proj * view);
    zShader.setUniform1f("farPlane", PREZ_FARPLANE);

    zBuffer.bind();
    renderer.clear();
    for (auto i : objects)
    {
        i->draw(zShader);
    }
    zBuffer.unbind();
}

void Engine::shadowPass(ShadowMap* shadowMaps, std::vector<Light*>& lights, std::vector<Model*>& objects)
{
    float aspect = 1.0f;
    glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, SHADOW_NEARPLANE, SHADOW_FARPLANE);
    shadowShader.setUniform1f("farPlane", SHADOW_FARPLANE);

    for (int i = 0; i < lights.size(); i++)
    {
        glm::mat4 shadowTransforms[6];
        glm::vec3 lightPos = lights[i]->pos;
        shadowTransforms[0] = shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0));
        shadowTransforms[1] = shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0));
        shadowTransforms[2] = shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0));
        shadowTransforms[3] = shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0));
        shadowTransforms[4] = shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0));
        shadowTransforms[5] = shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0));

        shadowMaps[i].bind();
        renderer.clear(0.0f, 0.0f, 0.0f);
        for (int i = 0; i < 6; i++)
        {
            shadowShader.setUniformMat4(("shadowMatrices[" + std::to_string(i) + "]").c_str(), shadowTransforms[i]);
        }
        shadowShader.setUniformVec3("lightPos", lightPos);

        for (auto i : objects) i->draw(shadowShader);

        shadowMaps[i].unbind();
    }
}

void Engine::renderPass()
{
    screenFB->bind();

    renderer.clear(0.0, 0.0, 0.0);
    glm::mat4 proj = glm::perspective(glm::radians(camera.FOV()), (float)this->windowWidth() / (float)this->windowHeight(), 0.1f, 100.0f);
    shader.setUniformMat4("proj", proj);
    shader.setUniformMat4("view", camera.getViewMatrix());
    shader.setUniformVec3("viewPos", camera.pos());
    shader.setUniform1f("normDir", 1.0);
    shader.setLight(lights);

    //shader.setTexture("ordTex", ordTex, ordTex.slot);
    shader.setMaterial(matAlbedo, matMetallic, matRoughness, matAo);
    shader.setUniform1i("useTexture", useTexture);
    shader.setUniform1i("useNormalMap", useNormalMap);
    //shader.setTexture("material.normalMap", normMap, normMap.slot);
    shader.setTexture("material.reflMap", skybox.texture(), skybox.texture().slot);
    shader.setUniform1i("useReflMap", useReflMap);
    shader.setUniform1f("material.reflStrength", reflStrength);
    shader.setUniform1i("shadowOn", shadowOn);
    for (int i = 0; i < lights.size(); i++)
    {
        shadowMapPoint[i].linkTextureUnit(i + 8);
        shader.setUniform1i(("depthMapPoint[" + std::to_string(i) + "]").c_str(), i + 8);
    }
    shader.setUniform1f("shadowFarPlane", SHADOW_FARPLANE);

    for (auto i : objects) i->draw(shader);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0, 0.0, -5.0f));
    model = glm::scale(model, glm::vec3(40.0f, 40.0f, 40.0f));
    glm::vec3 wallColor(1.0f, 1.0f, 1.0f);
    shader.setUniform1i("useReflMap", 0);
    shader.setUniform1i("useNormalMap", 0);
    shader.setUniformMat4("model", model);
    shader.setMaterial(wallColor, 0.0f, 1.0f, 0.5f);
    renderer.draw(square->VA(), shader);

    lightShader.setUniformMat4("proj", proj);
    lightShader.setUniformMat4("view", camera.getViewMatrix());

    for (int i = 0; i < lights.size(); i++)
    {
        model = glm::translate(glm::mat4(1.0f), lights[i]->pos);
        lightShader.setUniformMat4("model", model);
        lightShader.setUniformVec3("lightColor", glm::length(lights[i]->color) * glm::normalize(lights[i]->color));
        renderer.draw(sphere->VA(), lightShader);
    }

    //skybox.setProjection(proj);
    //skybox.setView(glm::mat4(glm::mat3(camera.getViewMatrix())));
    //skybox.draw();
    screenFB->unbind();
}

void Engine::postPass()
{
    renderer.clear();
    scrShader.setTexture("frameBuffer", *screenFBTex, screenFBTex->slot);
    scrShader.setUniform1f("gamma", gamma);
    scrShader.setUniform1f("exposure", exposure);
    renderer.draw(screenVA, scrShader);
}

void Engine::renderGUI()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    {
        ImGui::Begin("TEST");
        {
            ImGui::Text("Light0");
            ImGui::ColorEdit3("Color0", (float*)&lights[0]->color);
            ImGui::SliderFloat3("Pos0", (float*)&lights[0]->pos, -20.0f, 20.0f);
            ImGui::SliderFloat("Strength", &lights[0]->strength, 0.1f, 100.0f);
            ImGui::Text("Light1");
            ImGui::ColorEdit3("Color1", (float*)&lights[1]->color);
            ImGui::SliderFloat3("Pos1", (float*)&lights[1]->pos, -20.0f, 20.0f);
            ImGui::Text("Light2");
            ImGui::ColorEdit3("Color2", (float*)&lights[2]->color);
            ImGui::SliderFloat3("Pos2", (float*)&lights[2]->pos, -20.0f, 20.0f);
            ImGui::SliderInt("Vertical Sync", &verticalSync, 0, 1);
            ImGui::ColorEdit3("Albedo", (float*)&matAlbedo);
            ImGui::SliderFloat("Metallic", &matMetallic, 0.0f, 1.0f);
            ImGui::SliderFloat("Roughness", &matRoughness, 0.0f, 1.0f);
            ImGui::SliderFloat("Ao", &matAo, 0.0f, 1.0f);
            ImGui::SliderInt("Shadow On", &shadowOn, 0, 1);
            ImGui::SliderInt("Texture on", &useTexture, 0, 1);
            ImGui::SliderInt("NormalMap on", &useNormalMap, 0, 1);
            ImGui::SliderInt("ReflMap on", &useReflMap, 0, 1);
            ImGui::SliderFloat("ReflStrength", &reflStrength, 0.0f, 1.0f);
            ImGui::SliderFloat("Gamma", &gamma, 1.0f, 4.0f);
            ImGui::SliderFloat("Exposure", &exposure, 0.01f, 20.0f);
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
}
