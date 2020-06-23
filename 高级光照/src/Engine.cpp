#include "Engine.h"

char modelPathBuf[128] = { 0 };
char shaderPathBuf[128] = { 0 };
char scenePathBuf[128] = { 0 };
int windowSizeBuf[2] = { 0 };
const char* defaultModelPath = "res/model/";
const char* defaultShaderPath = "res/shader/";
const char* defaultScene = "res/test.sc";

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
    enableZCull(true),
    objectIndexGUI(0),
    lightIndexGUI(0),
    currentScene(defaultScene),
    forceFlatNormals(false)
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

    this->setupFrameBuffersAndTextures();
    this->setupScene();
    this->setupShaders();
    this->setupGUI();

    skybox.loadSphere("res/texture/017.hdr", GL_SRGB);
}

void Engine::renderLoop()
{
    this->processKey(0, 0, 0, 0);

    renderer.clear(0.0f, 0.0f, 0.0f);

    if (shadowOn) this->shadowPass(shadowMapPoint, lights, objects);
    
    this->ZPass(*ZMap, objects);
    
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
        delete ZMap;
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
        delete ZMap;
    }
    screenFB = new FrameBuffer;
    screenRB = new RenderBuffer;
    screenFBTex = new Texture;
    ZMap = new DepthMap;

    screenFB->generate(width, height);
    screenRB->allocate(GL_DEPTH24_STENCIL8, width, height);
    screenFBTex->generate2D(width, height, GL_RGB16F);
    screenFB->attachRenderBuffer(GL_DEPTH_STENCIL_ATTACHMENT, *screenRB);
    screenFB->attachTexture(GL_COLOR_ATTACHMENT0, *screenFBTex);

    ZMap->init(DepthMap::SCREEN_SPACE, width, height, GL_DEPTH_COMPONENT32F);
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
        shadowMapPoint[i].init(DepthMap::CUBE, 1024, 1024, GL_DEPTH_COMPONENT32F);
    }

    this->resetScreenBuffer(this->windowWidth(), this->windowHeight());
}

void Engine::setupScene()
{
    square = new Square();
    square->addTangents();
    square->setupVA();
    sphere = new Sphere(20, 10, 1.0f, Shape::VERTEX);
    sphere->addTangents();
    sphere->setupVA();

    screenVB.allocate(sizeof(SCREEN_COORD), SCREEN_COORD, 6);
    screenVA.addBuffer(screenVB, LAYOUT_POS2);

    if (!SceneLoader::loadScene(objects, materials, lights, "res/test.sc"))
    {
        this->setTerminateStatus(true);
    }
}

void Engine::setupShaders()
{
    shader = new Shader("res/shader/PBR.shader");
    lightShader.load("res/shader/light.shader");
    skyboxShader.load("res/shader/skyboxSphere.shader");
    scrShader.load("res/shader/frameBuffer.shader");
    shadowShader.load("res/shader/shadowMapPoint.shader");
    zShader.load("res/shader/Zpass.shader");

    zShader.setUniform1f("farPlane", camera.farPlane());
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

void Engine::ZPass(DepthMap& zMap, std::vector<Model*> objects)
{
    this->setViewport(0, 0, this->windowWidth(), this->windowHeight());

    glm::mat4 proj = camera.projMatrix(this->windowWidth(), this->windowHeight());
    glm::mat4 view = camera.viewMatrix();

    zShader.setUniformMat4("VPmatrix", proj * view);

    zMap.bind();
    renderer.clear();
    for (auto i : objects)
    {
        i->draw(zShader);
    }
    zMap.unbind();
}

void Engine::shadowPass(DepthMap* shadowMaps, std::vector<Light*>& lights, std::vector<Model*>& objects)
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

        this->setViewport(0, 0, shadowMaps[i].width(), shadowMaps[i].height());
        renderer.clear();

        shadowShader.setUniformVec3("lightPos", lightPos);
        for (int i = 0; i < 6; i++)
        {
            shadowShader.setUniformMat4(("shadowMatrices[" + std::to_string(i) + "]").c_str(), shadowTransforms[i]);
        }

        for (auto i : objects) i->draw(shadowShader);

        shadowMaps[i].unbind();
    }
}

void Engine::ssShadowPass(DepthMap* ssShadow, DepthMap* ssDepth, DepthMap* shadowMaps)
{
}

void Engine::renderPass()
{
    screenFB->bind();

    this->setViewport(0, 0, this->windowWidth(), this->windowHeight());
    renderer.clear(0.0, 0.0, 0.0);
    glm::mat4 proj = camera.projMatrix(this->windowWidth(), this->windowHeight());
    glm::mat4 view = camera.viewMatrix();

    shader->setUniform1f("nearPlane", camera.nearPlane());
    shader->setUniform1f("shadowFarPlane", SHADOW_FARPLANE);
    shader->setUniform1f("shadowNearPlane", SHADOW_NEARPLANE);
    shader->setUniform2f("viewport", this->windowWidth(), this->windowHeight());
    shader->setUniformMat4("VPmatrix", proj * view);
    shader->setUniformVec3("viewPos", camera.pos());
    shader->setLight(lights);

    //shader.setTexture("ordTex", ordTex, ordTex.slot);
    shader->setUniform1i("useTexture", useTexture);
    shader->setUniform1i("useNormalMap", useNormalMap);
    //shader.setTexture("material.normalMap", normMap, normMap.slot);
    //shader.setTexture("material.reflMap", skybox.texture(), skybox.texture().slot);
    //shader.setUniform1i("useReflMap", useReflMap);
    //shader.setUniform1f("material.reflStrength", reflStrength);
    shader->setUniform1i("shadowOn", shadowOn);
    shader->setUniform1i("forceFlatNormals", forceFlatNormals);

    shader->setTexture("ZMap", ZMap->texture(), 16);
    shader->setUniform1f("ZMapFarPlane", PREZ_FARPLANE);
    shader->setUniform1i("enableZCull", enableZCull);

    for (int i = 0; i < lights.size(); i++)
    {
        shadowMapPoint[i].linkTextureUnit(i + 8);
        shader->setUniform1i(("shadowMapPoint[" + std::to_string(i) + "]").c_str(), i + 8);
    }

    for (int i = 0; i < objects.size(); i++)
    {
        shader->setUniformMat3("modelInv", glm::transpose(glm::inverse(objects[i]->modelMatrix())));
        shader->setMaterial(materials[i]);
        objects[i]->draw(*shader);
    }

    //shader.setUniform1i("useReflMap", 0);
    shader->setUniform1i("useNormalMap", 0);

    lightShader.setUniformMat4("proj", proj);
    lightShader.setUniformMat4("view", camera.viewMatrix());

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
                ImGui::ColorEdit3("Color", (float*)&lit->color);
                ImGui::DragFloat3("Direction", (float*)&lit->dir, 0.01f, -1.0f, 1.0f);
                ImGui::DragFloat("Cutoff", &lit->cutoff, 0.1f, 0.0f, lit->outerCutoff);
                ImGui::DragFloat("OuterCutoff", &lit->outerCutoff, 0.1f, lit->cutoff, 180.0f);
                ImGui::DragFloat("Size", &lit->size, 0.01f, 0.01f, 3.0f);
                ImGui::DragFloat("Strength", &lit->strength, 0.01f, -10.0f, 10.0f);
                ImGui::DragFloat3("Position", (float*)&lit->pos, 0.1f);
            }
            if (ImGui::Button("New Light"))
            {
                if (lights.size() < MAX_LIGHTS)
                {
                    Light* lit = new Light({ 0.0f, 0.0f, 5.0f }, { 1.0f, 1.0f, 1.0f });
                    lightIndexGUI = lights.size();
                    this->addLight(lit);
                }
            }
            if (ImGui::Button("Remove Current"))
            {
                this->removeLight(lightIndexGUI);
                lightIndexGUI = 0;
            }
        }
        ImGui::End();
        ImGui::Begin("Modify Models");
        {
            if (objects.size() > 0)
            {
                ImGui::Text("Select Object");
                ImGui::SliderInt("Object", &objectIndexGUI, 0, objects.size() - 1);
                Model* obj = objects[objectIndexGUI];
                ImGui::Text(obj->name().c_str());
                glm::vec3 pos = obj->pos();
                ImGui::DragFloat3("Position", (float*)&pos, 0.1f);
                obj->setPos(pos);
                glm::vec3 scale = obj->scale();
                ImGui::DragFloat3("Scale", (float*)&scale, 0.1f);
                obj->setScale(scale);
                glm::vec3 rotation = obj->rotation();
                ImGui::DragFloat3("Rotation", (float*)&rotation, 0.1f);
                obj->setRotation(rotation);

                MaterialPBR& mat = materials[objectIndexGUI];
                ImGui::ColorEdit3("Albedo", (float*)&mat.albedo);
                ImGui::DragFloat("Metallic", &mat.metallic, 0.01f, 0.0f, 1.0f);
                ImGui::DragFloat("Roughness", &mat.roughness, 0.01f, 0.0f, 1.0f);
                ImGui::DragFloat("Ao", &mat.ao, 0.01f, 0.0f, 1.0f);
            }

            ImGui::InputText("Path", modelPathBuf, 128);
            if (ImGui::Button("Load"))
            {
                Model* newObj = new Model();
                if (newObj->loadModel(modelPathBuf))
                {
                    objectIndexGUI = objects.size();
                    this->addObject(newObj);
                }
                else delete newObj;
            }
            if (ImGui::Button("Remove Current"))
            {
                this->removeObject(objectIndexGUI);
                objectIndexGUI = 0;
            }
        }
        ImGui::End();
        ImGui::Begin("Shader & Scene");
        {
            ImGui::Text("Current Main Pass Shader:");
            ImGui::Text(shader->name().c_str());
            ImGui::InputText("Shader File", shaderPathBuf, 128);
            if (ImGui::Button("Reload"))
            {
                delete shader;
                shader = new Shader();
                if (!shader->load(shaderPathBuf))
                {
                    shader->load("res/shader/PBR.shader");
                }
            }
            ImGui::Text("Current Scene File:");
            ImGui::Text(currentScene.c_str());
            ImGui::InputText("Scene File", scenePathBuf, 128);
            if (ImGui::Button("Reload Scene"))
            {
                this->clearScene();
                if (!SceneLoader::loadScene(objects, materials, lights, scenePathBuf))
                {
                    SceneLoader::loadScene(objects, materials, lights, defaultScene);
                }
            }
            if (ImGui::Button("Save"))
            {
                SceneLoader::saveScene(objects, materials, lights, scenePathBuf);
            }
        }
        ImGui::End();
        ImGui::Begin("Test Options");
        {
            ImGui::Checkbox("Shadow", &shadowOn);
            ImGui::Checkbox("Texture", &useTexture);
            ImGui::Checkbox("NormalMap", &useNormalMap);
            ImGui::Checkbox("FlatNormals", &forceFlatNormals);
            //ImGui::Checkbox("ReflMap", &useReflMap);
            //ImGui::SliderFloat("ReflStrength", &reflStrength, 0.0f, 1.0f);
            ImGui::Checkbox("ZCull", &enableZCull);
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

void Engine::addObject(Model* object, MaterialPBR material)
{
    objects.push_back(object);
    materials.push_back(material);
}

void Engine::removeObject(int objectIndex)
{
    if (objectIndex >= objects.size() || objects.size() < 1) return;
    std::vector<MaterialPBR>::iterator itp = materials.begin();
    for (std::vector<Model*>::iterator it = objects.begin(); it != objects.end(); it++, itp++)
    {
        if (*it == objects[objectIndex])
        {
            delete objects[objectIndex];
            objects.erase(it);
            materials.erase(itp);
            break;
        }
    }
}

void Engine::addLight(Light* light)
{
    lights.push_back(light);
}

void Engine::removeLight(int lightIndex)
{
    if (lightIndex >= lights.size() || lights.size() < 1) return;
    for (std::vector<Light*>::iterator it = lights.begin(); it != lights.end(); it++)
    {
        if (*it == lights[lightIndex])
        {
            delete lights[lightIndex];
            lights.erase(it);
            return;
        }
    }
}

void Engine::clearScene()
{
    for (auto i : objects) delete i;
    objects.clear();
    for (auto i : lights) delete i;
    lights.clear();
    materials.clear();
    objectIndexGUI = lightIndexGUI = 0;
}
