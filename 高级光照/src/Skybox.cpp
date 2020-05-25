#include "Skybox.h"
#include "Renderer.h"

Shader Skybox::m_Shader;

Skybox::~Skybox()
{
	if (m_Shape != nullptr) delete m_Shape;
}

void Skybox::loadSphere(const char* filePath, GLuint colorType)
{
	m_Type = SPHERE;
	m_SkyboxTex = new Texture;
	m_SkyboxTex->loadSingle(filePath, colorType);
	m_Shader.load("res/shader/skyboxSphere.shader");
	m_Shader.setTexture("sky", *m_SkyboxTex, 0);
	m_Shape = new Sphere(30, 15, 1.0f, Shape::VERTEX);
	m_Shape->setupVA();
}

void Skybox::setProjection(const glm::mat4& projMatrix)
{
	m_Shader.setUniformMat4("proj", projMatrix);
}

void Skybox::setView(const glm::mat4& viewMatrix)
{
	m_Shader.setUniformMat4("view", viewMatrix);
}

void Skybox::draw()
{
	m_Shader.setTexture("sky", *m_SkyboxTex, 0);
	renderer.draw(m_Shape->VA(), m_Shader);
}

void Skybox::draw(const Texture& tex)
{
	m_Shader.setTexture("sky", tex, 0);
	renderer.draw(m_Shape->VA(), m_Shader);
}
