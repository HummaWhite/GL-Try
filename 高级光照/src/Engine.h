#pragma once

#include "EngineBase.h"

const float SHADOW_FARPLANE = 100.0f;
const float SHADOW_NEARPLANE = 0.1f;
const float PREZ_FARPLANE = 100.0f;
const int LIGHT_COUNT = 3;

class Engine :
	public EngineBase
{
public:
	Engine(int width, int height);
	~Engine();

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
	void setupLights();
	void setupObjects();
	void setupShaders();
	void setupGUI();

	void ZPass(FrameBuffer& zBuffer, std::vector<Model*> objects);
	void shadowPass(ShadowMap* shadowMaps, std::vector<Light*>& lights, std::vector<Model*>& objects);
	void renderPass();
	void postPass();
	void renderGUI();

private:
	Camera camera;
	std::vector<Model*> objects;
	std::vector<Light*> lights;
	Shader shader, scrShader, skyboxShader, lightShader, shadowShader, zShader;
	Skybox skybox;
	ShadowMap shadowMapPoint[LIGHT_COUNT];

	FrameBuffer* screenFB;
	RenderBuffer* screenRB;
	Texture* screenFBTex;

	Buffer screenVB;
	VertexArray screenVA;

	FrameBuffer* ZBuffer;
	Texture* ZBufferTex;

	Shape* square;
	Shape* sphere;

private:
	bool verticalSync;
	float gamma;
	float exposure;
	bool shadowOn;

	bool useTexture;
	bool useNormalMap;
	bool useReflMap;
	float reflStrength;

	std::vector<MaterialPBR> materials;
	int objectIndexGUI;
	int lightIndexGUI;

	bool enablePreZCull;

private:
	bool cursorDisabled;
	float lastCursorX, lastCursorY;
	bool firstCursorMove;
	bool F1Pressed;
};