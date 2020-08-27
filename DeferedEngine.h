#pragma once

#pragma once

#include "Includer.h"

class DeferedEngine :
	public EngineBase
{
public:
	DeferedEngine();
	~DeferedEngine();

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

	void prefilter(const char* envMapPath);

	void gBufferPass();
	void aoPass();
	void shadowPass();
	void renderPass();
	void postPass();
	void renderGUI();

private:
	Camera camera = Camera({ 0, -20, 8 });
	std::vector<Model*> objects;
	std::vector<Light*> lights;
	std::vector<MaterialPBR> materials;

	Shader* shader = nullptr, scrShader, lightShader, shadowShader, zShader;

	Skybox* skybox = nullptr;
	DepthMap shadowMapPoint[4];

	FrameBuffer* screenFB = nullptr;
	RenderBuffer* screenRB = nullptr;
	Texture* screenFBTex = nullptr;

	FrameBuffer* irradMapFB = nullptr;
	Texture* irradianceMap = nullptr;
	Shader envConvShader;

	FrameBuffer* prefilterFB = nullptr;
	Texture* prefilterMap = nullptr;
	Shader prefilterShader;

	FrameBuffer* gBufferFB = nullptr;
	RenderBuffer* gBufferRB = nullptr;
	Texture* ssAlbedoMap = nullptr;
	Texture* ssNormalMap = nullptr;
	Texture* ssDepMetRouMap = nullptr;
	Texture* ssPosMap = nullptr;
	Shader gBufferShader;

	FrameBuffer* ssaoFB = nullptr;
	DepthMap* ssaoMap = nullptr;
	Shader ssaoShader;

	DepthMap* ssaoBlurred = nullptr;
	Shader aoBlurShader;

	Texture lutMap;
	Texture noiseTex;

	Buffer screenVB;
	VertexArray screenVA;

	Shape* square = nullptr;
	Shape* sphere = nullptr;

	Texture ordTex;

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

	bool enableSSAO = true;
	float ssaoRadius = 1.0f;

	bool enableIBL = true;

private:
	bool cursorDisabled = true;
	float lastCursorX, lastCursorY;
	bool firstCursorMove = true;
	bool F1Pressed = false;
};