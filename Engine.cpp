#include "Engine.h"

const float SHADOW_FARPLANE = 100.0f;
const float SHADOW_NEARPLANE = 0.1f;
const float PREZ_FARPLANE = 100.0f;
const int MAX_LIGHTS = 4;

Engine::Engine() :
    EngineBase(),
    lastCursorX(this->windowWidth() / 2),
    lastCursorY(this->windowHeight() / 2),
    currentScene(defaultScene)
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

    skybox.loadSphere("res/texture/017.hdr");
}

void Engine::renderLoop()
{
    this->processKey(0, 0, 0, 0);

    renderer.clear(0.0f, 0.0f, 0.0f);

    this->ssZPass();

    if (shadowOn)
    {
        this->shadowPass();
    }
    
    this->renderPass();

    //this->GPass();
    //this->renderPassDefered();
    
    this->postPass();
    
    if (!cursorDisabled) this->renderGUI();
    
    VerticalSyncStatus(verticalSync);

    this->swapBuffers();
    this->display();
}

void Engine::terminate()
{
    if (screenFB != nullptr)
    {
        delete screenFB;
        delete screenRB;
        delete screenFBTex;
        delete ssZMap;
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
        delete ssZMap;
    }

    screenFB = new FrameBuffer;
    screenRB = new RenderBuffer;
    screenFBTex = new Texture;
    ssZMap = new DepthMap;

    screenFB->generate(width, height);
    screenRB->allocate(GL_DEPTH24_STENCIL8, width, height);
    screenFBTex->generate2D(width, height, GL_RGB16F);
    screenFBTex->setFilterAndWrapping(GL_LINEAR, GL_CLAMP_TO_EDGE);

    screenFB->attachRenderBuffer(GL_DEPTH_STENCIL_ATTACHMENT, *screenRB);
    screenFB->attachTexture(GL_COLOR_ATTACHMENT0, *screenFBTex);

    ssZMap->init(DepthMap::SCREEN_SPACE, width, height, GL_DEPTH_COMPONENT32F);
}

void Engine::processKey(int key, int scancode, int action, int mode)
{
    if (this->getKeyStatus(GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
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

    tex.loadSingle("res/texture/pixel.png", GL_RGB);
    tex.setFilterAndWrapping(GL_LINEAR, GL_CLAMP_TO_EDGE);
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
    lightShader.load("res/shader/lightDisplay.shader");
    scrShader.load("res/shader/postEffects.shader");
    shadowShader.load("res/shader/shadowMapPoint.shader");
    zShader.load("res/shader/ssDepth.shader");
    gShader.load("res/shader/GBuffer.shader");
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

void Engine::ssZPass()
{
    this->setViewport(0, 0, this->windowWidth(), this->windowHeight());

    glm::mat4 proj = camera.projMatrix(this->windowWidth(), this->windowHeight());
    glm::mat4 view = camera.viewMatrix();

    zShader.setMat4("VPmatrix", proj * view);
    zShader.set1f("farPlane", camera.farPlane());

    ssZMap->bind();
    renderer.clear();
    for (auto i : objects)
    {
        i->draw(zShader);
    }
    ssZMap->unbind();
}

void Engine::shadowPass()
{
    float aspect = 1.0f;
    glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, SHADOW_NEARPLANE, SHADOW_FARPLANE);
    shadowShader.set1f("farPlane", SHADOW_FARPLANE);

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

        shadowMapPoint[i].bind();

        this->setViewport(0, 0, shadowMapPoint[i].width(), shadowMapPoint[i].height());
        renderer.clear();

        shadowShader.setVec3("lightPos", lightPos);
        for (int i = 0; i < 6; i++)
        {
            shadowShader.setMat4(("lightVP[" + std::to_string(i) + "]").c_str(), shadowTransforms[i]);
        }

        for (auto i : objects) i->draw(shadowShader);

        shadowMapPoint[i].unbind();
    }
}

void Engine::GPass()
{
    if (enableZCull) glDisable(GL_DEPTH_TEST);
    gBufferFB->bind();

    this->setViewport(0, 0, this->windowWidth(), this->windowHeight());
    renderer.clear(0.0, 0.0, 0.0);
    glm::mat4 proj = camera.projMatrix(this->windowWidth(), this->windowHeight());
    glm::mat4 view = camera.viewMatrix();

    gShader.resetTextureMap();
    gShader.setMat4("VPmatrix", proj * view);
    gShader.set1i("useNormalMap", useNormalMap);
    gShader.set1i("forceFlatNormals", forceFlatNormals);
    gShader.set1i("enableZCull", enableZCull);
    gShader.setTexture("ssZMap", ssZMap->texture());
    gShader.set1f("ssZMapDivisor", PREZ_FARPLANE);
    gShader.set1f("posDivisor", posDivisor);
    gShader.set2f("viewport", this->windowWidth(), this->windowHeight());

    for (int i = 0; i < objects.size(); i++)
    {
        gShader.setMat3("modelInv", glm::transpose(glm::inverse(objects[i]->modelMatrix())));
        gShader.setMaterial(materials[i]);
        objects[i]->draw(gShader);
    }

    gBufferFB->unbind();

    glEnable(GL_DEPTH_TEST);
}

void Engine::renderPass()
{
    if (enableZCull) glDisable(GL_DEPTH_TEST);
    screenFB->bind();

    this->setViewport(0, 0, this->windowWidth(), this->windowHeight());
    renderer.clear(0.0, 0.0, 0.0);
    glm::mat4 proj = camera.projMatrix(this->windowWidth(), this->windowHeight());
    glm::mat4 view = camera.viewMatrix();

    shader->resetTextureMap();
    shader->set1f("nearPlane", camera.nearPlane());
    shader->set1f("shadowFarPlane", SHADOW_FARPLANE);
    shader->set1f("shadowNearPlane", SHADOW_NEARPLANE);
    shader->set2f("viewport", this->windowWidth(), this->windowHeight());
    shader->setMat4("VPmatrix", proj * view);
    shader->setVec3("viewPos", camera.pos());
    shader->setLight(lights);

    shader->set1i("useTexture", useTexture);
    shader->set1i("useNormalMap", useNormalMap);
    shader->setTexture("ordTex", tex);
    shader->set1i("shadowOn", shadowOn);
    shader->set1i("forceFlatNormals", forceFlatNormals);

    shader->setTexture("ssZMap", ssZMap->texture());
    shader->set1f("ssZMapFar", PREZ_FARPLANE);
    shader->set1i("enableZCull", enableZCull);

    for (int i = 0; i < lights.size(); i++)
    {
        shader->setTexture(("shadowMapPoint[" + std::to_string(i) + "]").c_str(), shadowMapPoint[i].texture());
    }

    for (int i = 0; i < objects.size(); i++)
    {
        shader->setMat3("modelInv", glm::transpose(glm::inverse(objects[i]->modelMatrix())));
        shader->setMaterial(materials[i]);
        objects[i]->draw(*shader);
    }

    glEnable(GL_DEPTH_TEST);

    lightShader.setMat4("proj", proj);
    lightShader.setMat4("view", camera.viewMatrix());

    for (int i = 0; i < lights.size(); i++)
    {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), lights[i]->pos);
        model = glm::scale(model, glm::vec3(lights[i]->size));
        lightShader.setMat4("model", model);
        lightShader.setVec3("lightColor", glm::length(lights[i]->color) * glm::normalize(lights[i]->color));
        renderer.draw(sphere->VA(), lightShader);
    }

    /*skybox.setProjection(proj);
    skybox.setView(glm::mat4(glm::mat3(camera.getViewMatrix())));
    skybox.draw();*/
    screenFB->unbind();
}

void Engine::renderPassDefered()
{
    glDisable(GL_DEPTH_TEST);
    screenFB->bind();

    this->setViewport(0, 0, this->windowWidth(), this->windowHeight());
    renderer.clear(0.0, 0.0, 0.0);

    shader->resetTextureMap();
    shader->set1f("nearPlane", camera.nearPlane());
    shader->set1f("shadowFarPlane", SHADOW_FARPLANE);
    shader->set1f("shadowNearPlane", SHADOW_NEARPLANE);
    shader->setVec3("viewPos", camera.pos());
    shader->setLight(lights);
    shader->set1i("shadowOn", shadowOn);

    shader->setTexture("posTex", *posTex);
    shader->setTexture("normTex", *normTex);
    shader->setTexture("albedoTex", *albedoTex);
    shader->setTexture("metRouAoTex", *metRouAoTex);

    renderer.draw(screenVA, *shader);

    for (int i = 0; i < lights.size(); i++)
    {
        shader->setTexture(("shadowMapPoint[" + std::to_string(i) + "]").c_str(), shadowMapPoint[i].texture());
    }

    lightShader.setMat4("proj", camera.projMatrix(this->windowWidth(), this->windowHeight()));
    lightShader.setMat4("view", camera.viewMatrix());

    for (int i = 0; i < lights.size(); i++)
    {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), lights[i]->pos);
        model = glm::scale(model, glm::vec3(lights[i]->size));
        lightShader.setMat4("model", model);
        lightShader.setVec3("lightColor", glm::length(lights[i]->color) * glm::normalize(lights[i]->color));
        renderer.draw(sphere->VA(), lightShader);
    }

    screenFB->unbind();
    glEnable(GL_DEPTH_TEST);
}

void Engine::postPass()
{
    this->setViewport(0, 0, this->windowWidth(), this->windowHeight());
    renderer.clear();

    scrShader.resetTextureMap();
    //scrShader.setTexture("frameBuffer", ssZMap->texture(), 12);
    scrShader.setTexture("frameBuffer", *screenFBTex);
    //scrShader.setTexture("frameBuffer", *posTex);
    scrShader.set1f("gamma", gamma);
    scrShader.set1f("exposure", exposure);
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
