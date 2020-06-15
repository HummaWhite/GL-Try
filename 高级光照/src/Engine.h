#pragma once

#include "EngineBase.h"

const float SHADOW_FARPLANE = 100.0f;
const float SHADOW_NEARPLANE = 0.1f;
const float PREZ_FARPLANE = 100.0f;
const int MAX_LIGHTS = 4;

class Engine :
	public EngineBase
{
public:
	Engine();
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
	std::vector<MaterialPBR> materials;
	Shader* shader, scrShader, skyboxShader, lightShader, shadowShader, zShader;
	Skybox skybox;
	ShadowMap shadowMapPoint[MAX_LIGHTS];

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

	int objectIndexGUI;
	int lightIndexGUI;

	bool enablePreZCull;

private:
	bool cursorDisabled;
	float lastCursorX, lastCursorY;
	bool firstCursorMove;
	bool F1Pressed;
};