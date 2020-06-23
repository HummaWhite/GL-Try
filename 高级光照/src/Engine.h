#pragma once

#include "EngineBase.h"
#include "SceneLoader.h"

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

	void ZPass(DepthMap &zMap, std::vector<Model*> objects);
	void shadowPass(DepthMap* shadowMaps, std::vector<Light*>& lights, std::vector<Model*>& objects);
	void ssShadowPass(DepthMap* ssShadow, DepthMap* ssDepth, DepthMap* shadowMaps);
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
	DepthMap shadowMapPoint[MAX_LIGHTS];
	DepthMap* ZMap, * ssShadowMap;

	FrameBuffer* screenFB;
	RenderBuffer* screenRB;
	Texture* screenFBTex;

	Buffer screenVB;
	VertexArray screenVA;

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

	bool enableZCull;
	bool forceFlatNormals;

	std::string currentScene;

private:
	bool cursorDisabled;
	float lastCursorX, lastCursorY;
	bool firstCursorMove;
	bool F1Pressed;
};