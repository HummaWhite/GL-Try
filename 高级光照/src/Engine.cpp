#include "Engine.h"

const MaterialPBR material0 =
{
    { 0.3f, 0.6f, 1.0f }, 1.0f, 1.0f, 0.1f
};

const MaterialPBR material1 =
{
    { 1.0f, 1.0f, 1.0f }, 0.0f, 1.0f, 0.5f
};

char modelPathBuf[128] = { 0 };
char shaderPathBuf[128] = { 0 };
int windowSizeBuf[2] = { 0 };
const char* defaultModelPath = "res/model/";
const char* defaultShaderPath = "res/shader/";

Engine::Engine() :
    EngineBase(),
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
    cursorDisabled(true),
    lastCursorX(this->windowWidth() / 2),
    lastCursorY(this->windowHeight() / 2),
    firstCursorMove(true),
    F1Pressed(false),
    enablePreZCull(true),
    objectIndexGUI(0),
    lightIndexGUI(0)
{
    windowSizeBuf[0] = this->windowWidth();
    windowSizeBuf[1] = this->windowHeight();
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

    this->setupLights();
    this->setupFrameBuffersAndTextures();
    this->setupObjects();
    this->setupShaders();
    this->setupGUI();

    skybox.loadSphere("res/texture/017.hdr", GL_SRGB);
}

void Engine::renderLoop()
{
    this->processKey(0, 0, 0, 0);

    renderer.clear(0.0f, 0.0f, 0.0f);

    if (shadowOn) this->shadowPass(shadowMapPoint, lights, objects);
    
    this->ZPass(*ZBuffer, objects);
    
    this->renderPass();
    
    this->postPass();
    
    if (!cursorDisabled) this->renderGUI();
    
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
        delete ZBuffer;
        delete ZBufferTex;
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void Engine::resetScreenBuffer(int width, int height)
{
    if (screenFB != nullptr)
    {
        delete screenFB;
        delete screenRB;
        delete screenFBTex;
        delete ZBuffer;
        delete ZBufferTex;
    }
    screenFB = new FrameBuffer;
    screenRB = new RenderBuffer;
    screenFBTex = new Texture;
    ZBuffer = new FrameBuffer;
    ZBufferTex = new Texture;

    screenFB->generate(width, height);
    screenRB->allocate(GL_DEPTH24_STENCIL8, width, height);
    screenFBTex->generate2D(width, height, GL_RGB16F);
    screenFB->attachRenderBuffer(GL_DEPTH_STENCIL_ATTACHMENT, *screenRB);
    screenFB->attachTexture(GL_COLOR_ATTACHMENT0, *screenFBTex);

    ZBuffer->generate(width, height);
    ZBufferTex->generateDepth2D(width, height, GL_DEPTH_COMPONENT32F);
    ZBuffer->attachTexture(GL_DEPTH_ATTACHMENT, *ZBufferTex);
    ZBuffer->activateAttachmentTargets({ GL_NONE });
}

void Engine::processKey(int key, int scancode, int action, int mode)
{
    if (this->getKeyStatus(GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(this->window(), true);
        this->setTerminateStatus(true);
    }
    if (cursorDisabled)
    {
        if (this->getKeyStatus(GLFW_KEY_W) == GLFW_PRESS)
            camera.move(GLFW_KEY_W);
        if (this->getKeyStatus(GLFW_KEY_S) == GLFW_PRESS)
            camera.move(GLFW_KEY_S);
        if (this->getKeyStatus(GLFW_KEY_A) == GLFW_PRESS)
            camera.move(GLFW_KEY_A);
        if (this->getKeyStatus(GLFW_KEY_D) == GLFW_PRESS)
            camera.move(GLFW_KEY_D);
        if (this->getKeyStatus(GLFW_KEY_Q) == GLFW_PRESS)
            camera.move(GLFW_KEY_Q);
        if (this->getKeyStatus(GLFW_KEY_E) == GLFW_PRESS)
            camera.move(GLFW_KEY_E);
        if (this->getKeyStatus(GLFW_KEY_R) == GLFW_PRESS)
            camera.move(GLFW_KEY_R);
        if (this->getKeyStatus(GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            camera.move(GLFW_KEY_LEFT_SHIFT);
        if (this->getKeyStatus(GLFW_KEY_SPACE) == GLFW_PRESS)
            camera.move(GLFW_KEY_SPACE);
    }

    if (this->getKeyStatus(GLFW_KEY_F1) == GLFW_PRESS) F1Pressed = true;
    if (this->getKeyStatus(GLFW_KEY_F1) == GLFW_RELEASE)
    {
        if (F1Pressed)
        {
            if (cursorDisabled)
            {
                glfwSetInputMode(this->window(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }
            else
            {
                glfwSetInputMode(this->window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            }
            cursorDisabled ^= 1;
            F1Pressed = false;
        }
    }
}

void Engine::processCursor(float posX, float posY)
{
    if (!cursorDisabled) return;
    if (firstCursorMove == 1)
    {
        lastCursorX = posX;
        lastCursorY = posY;
        firstCursorMove = false;
        return;
    }

    float offsetX = (posX - lastCursorX) * CAMERA_ROTATE_SENSITIVITY;
    float offsetY = (posY - lastCursorY) * CAMERA_ROTATE_SENSITIVITY;
    glm::vec3 offset = glm::vec3(-offsetX, -offsetY, 0);
    camera.rotate(offset);

    lastCursorX = posX;
    lastCursorY = posY;
}

void Engine::processScroll(float offsetX, float offsetY)
{
    camera.changeFOV(offsetY);
}

void Engine::processResize(int width, int height)
{
    this->resizeWindow(width, height);
    this->resetScreenBuffer(width, height);
}

void Engine::setupFrameBuffersAndTextures()
{
    for (int i = 0; i < MAX_LIGHTS; i++)
    {
        shadowMapPoint[i].init(ShadowMap::POINT, 1024, GL_DEPTH_COMPONENT32F);
    }

    this->resetScreenBuffer(this->windowWidth(), this->windowHeight());
}

void Engine::setupLights()
{
    Light* light = new Light({ 0.0f, 0.0f, 14.0f }, { 1.0f, 1.0f, 1.0f });
    Light* light2 = new Light({ 6.0f, -6.0f, 0.0f }, { 0.7f, 0.0f, 0.8f });
    Light* light3 = new Light({ -4.0, -2.0f, -1.0f }, { 0.2f, 0.5f, 0.9f });
    lights.push_back(light);
    lights.push_back(light2);
    lights.push_back(light3);
}

void Engine::setupObjects()
{
    square = new Square();
    square->addTangents();
    square->setupVA();
    sphere = new Sphere(20, 10, 1.0f, Shape::VERTEX);
    sphere->addTangents();
    sphere->setupVA();

    screenVB.allocate(sizeof(SCREEN_COORD), SCREEN_COORD, 6);
    screenVA.addBuffer(screenVB, LAYOUT_POS2);

    Model* bunny = new Model("res/model/bunny.obj", { 0.5f, 0.5f, 8.0f }, 0.5f);
    objects.push_back(bunny);
    materials.push_back(material0);

    Model* nano = new Model("res/model/nanosuit/nanosuit.obj", { 2.0f, 0.0f, -5.0f });
    objects.push_back(nano);
    materials.push_back(material0);

    Model* floor = new Model(*square, { 0.0f, 0.0f, -5.0f }, 40.0f);
    objects.push_back(floor);
    materials.push_back(material1);

    Shape* teapot = new BezierCurves("res/model/teapotCGA.bpt", 20, 20, Shape::VERTEX);
    teapot->addTangents();
    teapot->setupVA();
    Model* pot = new Model(*teapot, { 8.0f, -4.0f, -5.0f });
    objects.push_back(pot);
    materials.push_back(material1);
}

void Engine::setupShaders()
{
    shader = new Shader("res/shader/PBR.shader");
    lightShader.load("res/shader/light.shader");
    skyboxShader.load("res/shader/skyboxSphere.shader");
    scrShader.load("res/shader/frameBuffer.shader");
    shadowShader.load("res/shader/shadowMapPoint.shader");
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
    this->setViewport(0, 0, this->windowWidth(), this->windowHeight());

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

        this->setViewport(0, 0, shadowMaps[i].size(), shadowMaps[i].size());
        renderer.clear(0.0f, 0.0f, 0.0f);

        shadowShader.setUniformVec3("lightPos", lightPos);
        for (int i = 0; i < 6; i++)
        {
            shadowShader.setUniformMat4(("shadowMatrices[" + std::to_string(i) + "]").c_str(), shadowTransforms[i]);
        }

        for (auto i : objects) i->draw(shadowShader);

        shadowMaps[i].unbind();
    }
}

void Engine::renderPass()
{
    screenFB->bind();

    this->setViewport(0, 0, this->windowWidth(), this->windowHeight());
    renderer.clear(0.0, 0.0, 0.0);
    glm::mat4 proj = glm::perspective(glm::radians(camera.FOV()), (float)this->windowWidth() / (float)this->windowHeight(), 0.1f, 100.0f);
    glm::mat4 view = camera.getViewMatrix();

    shader->setUniform2f("viewport", this->windowWidth(), this->windowHeight());
    shader->setUniformMat4("proj", proj);
    shader->setUniformMat4("view", view);
    shader->setUniformVec3("viewPos", camera.pos());
    shader->setUniform1f("normDir", 1.0);
    shader->setLight(lights);

    //shader.setTexture("ordTex", ordTex, ordTex.slot);
    shader->setUniform1i("useTexture", useTexture);
    shader->setUniform1i("useNormalMap", useNormalMap);
    //shader.setTexture("material.normalMap", normMap, normMap.slot);
    //shader.setTexture("material.reflMap", skybox.texture(), skybox.texture().slot);
    //shader.setUniform1i("useReflMap", useReflMap);
    //shader.setUniform1f("material.reflStrength", reflStrength);
    shader->setUniform1i("shadowOn", shadowOn);

    shader->setTexture("preZTex", *ZBufferTex, 16);
    shader->setUniform1f("preZFarPlane", PREZ_FARPLANE);
    shader->setUniform1i("enablePreZCull", enablePreZCull);

    for (int i = 0; i < lights.size(); i++)
    {
        shadowMapPoint[i].linkTextureUnit(i + 8);
        shader->setUniform1i(("shadowMapPoint[" + std::to_string(i) + "]").c_str(), i + 8);
    }
    shader->setUniform1f("shadowFarPlane", SHADOW_FARPLANE);

    for (int i = 0; i < objects.size(); i++)
    {
        shader->setMaterial(materials[i]);
        objects[i]->draw(*shader);
    }

    //shader.setUniform1i("useReflMap", 0);
    shader->setUniform1i("useNormalMap", 0);

    lightShader.setUniformMat4("proj", proj);
    lightShader.setUniformMat4("view", camera.getViewMatrix());

    for (int i = 0; i < lights.size(); i++)
    {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), lights[i]->pos);
        model = glm::scale(model, glm::vec3(lights[i]->size));
        lightShader.setUniformMat4("model", model);
        lightShader.setUniformVec3("lightColor", glm::length(lights[i]->color) * glm::normalize(lights[i]->color));
        renderer.draw(sphere->VA(), lightShader);
    }

    /*skybox.setProjection(proj);
    skybox.setView(glm::mat4(glm::mat3(camera.getViewMatrix())));
    skybox.draw();*/
    screenFB->unbind();
}

void Engine::postPass()
{
    this->setViewport(0, 0, this->windowWidth(), this->windowHeight());
    renderer.clear();
    //scrShader.setTexture("frameBuffer", *ZBufferTex, 12);
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
        ImGui::Begin("Modify Lights");
        {
            if (lights.size() > 0)
            {
                ImGui::Text("Select Light");
                ImGui::SliderInt("Light", &lightIndexGUI, 0, lights.size() - 1);
                Light* lit = lights[lightIndexGUI];
                ImGui::ColorEdit3("Color0", (float*)&lit->color);
                ImGui::DragFloat("Size0", &lit->size, 0.01f, 0.01f, 3.0f);
                ImGui::DragFloat("Strength0", &lit->strength, 0.1f, -10.0f, 10.0f);
                ImGui::SliderFloat3("Pos0", (float*)&lit->pos, -20.0f, 20.0f);
            }
            if (ImGui::Button("New Light"))
            {
                if (lights.size() < MAX_LIGHTS)
                {
                    Light* lit = new Light({ 0.0f, 0.0f, 5.0f }, { 1.0f, 1.0f, 1.0f });
                    lightIndexGUI = lights.size();
                    lights.push_back(lit);
                }
            }
            if (ImGui::Button("Remove Current"))
            {
                for (std::vector<Light*>::iterator it = lights.begin(); it != lights.end(); it++)
                {
                    if (*it == lights[lightIndexGUI])
                    {
                        delete lights[lightIndexGUI];
                        lights.erase(it);
                        lightIndexGUI = 0;
                        break;
                    }
                }
            }
        }
        ImGui::End();
        ImGui::Begin("Modify Models");
        {
            ImGui::Text("Select Object");
            ImGui::SliderInt("Object", &objectIndexGUI, 0, objects.size() - 1);
            Model* obj = objects[objectIndexGUI];
            ImGui::Text(obj->name().c_str());
            glm::vec3 pos = obj->pos();
            ImGui::DragFloat3("Position", (float*)&pos, 0.1f);
            obj->setPos(pos);
            glm::vec3 scale = obj->scale();
            ImGui::DragFloat3("Scale", (float*)&scale, 0.1f, 0.1f, 100.0f);
            obj->setScale(scale);
            glm::vec3 angle = obj->angle();
            ImGui::DragFloat3("Angle", (float*)&angle, 0.1f);
            obj->setAngle(angle);

            MaterialPBR& mat = materials[objectIndexGUI];
            ImGui::ColorEdit3("Albedo", (float*)&mat.albedo);
            ImGui::SliderFloat("Metallic", &mat.metallic, 0.0f, 1.0f);
            ImGui::SliderFloat("Roughness", &mat.roughness, 0.0f, 1.0f);
            ImGui::SliderFloat("Ao", &mat.ao, 0.0f, 1.0f);

            ImGui::InputText("Path", modelPathBuf, 128);
            if (ImGui::Button("Load"))
            {
                Model* newObj = new Model();
                if (newObj->loadModel(modelPathBuf))
                {
                    objectIndexGUI = objects.size();
                    objects.push_back(newObj);
                    materials.push_back(material0);
                }
                else delete newObj;
            }
        }
        ImGui::End();
        ImGui::Begin("Modify Main RenderPass Shader");
        {
            ImGui::Text("Current File:");
            ImGui::Text(shader->name().c_str());
            ImGui::InputText("Path", shaderPathBuf, 128);
            if (ImGui::Button("Reload"))
            {
                delete shader;
                shader = new Shader();
                if (!shader->load(shaderPathBuf))
                {
                    shader->load("res/shader/PBR.shader");
                }
            }
        }
        ImGui::End();
        ImGui::Begin("Test Options");
        {
            ImGui::Checkbox("Shadow", &shadowOn);
            ImGui::Checkbox("Texture", &useTexture);
            ImGui::Checkbox("NormalMap", &useNormalMap);
            //ImGui::Checkbox("ReflMap", &useReflMap);
            //ImGui::SliderFloat("ReflStrength", &reflStrength, 0.0f, 1.0f);
            ImGui::Checkbox("PreZCull", &enablePreZCull);
            ImGui::Checkbox("Vertical Sync", &verticalSync);
            ImGui::DragFloat("Gamma", &gamma, 0.01f, 1.0f, 4.0f);
            ImGui::DragFloat("Exposure", &exposure, 0.01f, 0.1f, 100.0f);

            ImGui::Text("Window Size: %d x %d", windowSizeBuf[0], windowSizeBuf[1]);
            ImGui::InputInt2("New Size", windowSizeBuf);
            if (ImGui::Button("Set")) this->processResize(windowSizeBuf[0], windowSizeBuf[1]);

            ImGui::Text("x: %.3f y: %.3f z: %.3f  FOV: %.1f", camera.pos().x, camera.pos().y, camera.pos().z, camera.FOV());
            ImGui::Text("Render Time: %.3f ms, FPS: %.3f", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::Text("\n");
            ImGui::Text("WASD / Lshift / Space  - move");
            ImGui::Text("Mouse                  - view");
            ImGui::Text("F1                     - release mouse");
            ImGui::Text("ESC                    - exit");
            if (ImGui::Button("Exit"))
            {
                this->setTerminateStatus(true);
            }
        }
        ImGui::End();
    }
    ImGui::EndFrame();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
