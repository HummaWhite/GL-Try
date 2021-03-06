#pragma once

#include "Includer.h"

class Engine :
	public EngineBase
{
public:
	Engine();
	~Engine();

	void addObject(Model* object, MaterialPBR material = Material::defaultMaterialPBR);
	void removeObject(int objectIndex);

	void addLight(Light* light);
	void removeLight(int lightIndex);

	void clearScene();

private:
	void init();
	void renderLoop();
	void terminate();

	void resetScreenBuffer(int width, int height);

	void processKey(int key, int scancode, int action, int mode);
	void processCursor(float posX, float posY);
	void processScroll(float offsetX, float offsetY);
	void processResize(int width, int height);

	void setupFrameBuffersAndTextures();
	void setupScene();
	void setupShaders();
	void setupGUI();

	void ssZPass();
	void shadowPass();
	void GPass();
	void renderPass();
	void renderPassDefered();
	void postPass();
	void renderGUI();

private:
	Camera camera = Camera({ 0, -20, 8 });
	std::vector<Model*> objects;
	std::vector<Light*> lights;
	std::vector<MaterialPBR> materials;
	Shader* shader = nullptr, scrShader, lightShader, shadowShader, zShader, gShader;
	Skybox skybox;
	DepthMap shadowMapPoint[4];
	DepthMap* ssZMap = nullptr;

	FrameBuffer* screenFB = nullptr, * gBufferFB = nullptr;
	RenderBuffer* screenRB = nullptr, * gBufferRB = nullptr;
	Texture* screenFBTex = nullptr, * posTex = nullptr, * normTex = nullptr, * albedoTex = nullptr, * metRouAoTex = nullptr;

	Buffer screenVB;
	VertexArray screenVA;

	Shape* square = nullptr;
	Shape* sphere = nullptr;

	Texture tex;

private:
	bool verticalSync = true;
	float gamma = 2.2f;
	float exposure = 2.0f;
	bool shadowOn = true;

	bool useTexture = false;
	bool useNormalMap = false;
	bool useReflMap = false;
	float reflStrength = 0.0f;

	int objectIndexGUI = 0;
	int lightIndexGUI = 0;

	float posDivisor = 100.0f;

	bool enableZCull = true;
	bool forceFlatNormals = false;

	std::string currentScene;

private:
	bool cursorDisabled = true;
	float lastCursorX, lastCursorY;
	bool firstCursorMove = true;
	bool F1Pressed = false;

private:
};