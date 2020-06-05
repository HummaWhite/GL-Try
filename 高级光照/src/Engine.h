#pragma once

#include "EngineBase.h"

const int SHADOW_RES = 1024;
const float SHADOW_FARPLANE = 50.0f;
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

	void resizeWindow();
	void resetScreenBuffer(int width, int height);

	void processKey();
	void processCursor();
	void processScroll();

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

	FrameBuffer ZBuffer;
	Texture ZBufferTex;

	Shape* square;
	Shape* sphere;
	Model bunny, nano;

private:
	int verticalSync;
	float gamma;
	float exposure;
	int shadowOn;

	int useTexture;
	int useNormalMap;
	int useReflMap;
	float reflStrength;

	glm::vec3 matAlbedo;
	float matMetallic;
	float matRoughness;
	float matAo;
};