#pragma once

#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

const GLuint NORMAL_ATTENUATION_LEVEL = 5;
const glm::vec3 ATTENUATION[] =
{
	glm::vec3(1, 0.7, 1.8),        //7
	glm::vec3(1, 0.35, 0.44),      //13
	glm::vec3(1, 0.22, 0.2),       //20
	glm::vec3(1, 0.14, 0.07),      //32
	glm::vec3(1, 0.09, 0.032),     //50
	glm::vec3(1, 0.07, 0.017),     //65
	glm::vec3(1, 0.045, 0.0075),   //100
	glm::vec3(1, 0.027, 0.0028),   //160
	glm::vec3(1, 0.022, 0.0019),   //200
	glm::vec3(1, 0.014, 0.0007),   //325
	glm::vec3(1, 0.007, 0.0002),   //600
	glm::vec3(1, 0.0014, 7e-006)   //3250
};

struct Light
{
public:
	enum
	{
		DIRECTIONAL,
		POINT,
		SPOT
	};
	glm::vec3 pos;
	glm::vec3 dir;
	glm::vec3 color;
	glm::vec3 attenuation;
	float cutOff;
	float outerCutOff;
	GLuint type;
	Light(glm::vec3 _dir, glm::vec3 _color) :
		type(Light::DIRECTIONAL), dir(_dir), color(_color) {}
	Light(GLuint _type, glm::vec3 _pos, glm::vec3 _color, GLuint attenLevel = NORMAL_ATTENUATION_LEVEL) :
		type(_type), pos(_pos), color(_color), attenuation(ATTENUATION[attenLevel]) {}
	Light(GLuint _type, glm::vec3 _pos, glm::vec3 _dir, glm::vec3 _color,
		float _cutOff, float _outerCutOff, GLuint attenLevel = NORMAL_ATTENUATION_LEVEL) :
		type(_type), pos(_pos), dir(_dir), color(_color), cutOff(cos(glm::radians(_cutOff))),
		outerCutOff(cos(glm::radians(_outerCutOff))), attenuation(ATTENUATION[attenLevel]) {}
};

class LightGroup
{
public:
	LightGroup() : m_Count(0) {}
	void add(Light* light)
	{
		m_Lights.push_back(light);
		m_Count++;
	}
	std::vector<Light*> lights() const
	{
		return m_Lights;
	}
	GLuint count() const { return m_Count; }
	Light* operator [] (int x) const { return m_Lights[x]; }
private:
	std::vector<Light*> m_Lights;
	GLuint m_Count;
};