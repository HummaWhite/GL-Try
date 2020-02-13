#pragma once

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

const float CAMERA_ROTATE_SENSITIVITY = 0.05f;
const float CAMERA_MOVE_SENSITIVITY = 0.1f;
const float CAMERA_FOV_SENSITIVITY = 1.0f;
const float CAMERA_PITCH_LIMIT = 88.0f;
const float CAMERA_FOV = 45.0f;
const glm::vec3 VEC_UP = glm::vec3(0.0f, 0.0f, 1.0f);

class Camera
{
public:
	Camera(glm::vec3 pos = glm::vec3(0, 0, 0), glm::vec3 angle = glm::vec3(glm::radians(90.0f), 0.0f, 0.0f))
		:m_Pos(pos), m_Angle(angle), m_FOV(CAMERA_FOV) {}

	void move(glm::vec3 vect);
	void move(GLuint key);
	void rotate(glm::vec3 angle);
	void changeFOV(float offset);

	float FOV() const { return m_FOV; }
	glm::mat4 getViewMatrix();
	glm::mat4 getViewMatrix(glm::vec3 targetPos);

	glm::vec3 m_Pos;
	glm::vec3 m_Angle;
private:
	glm::vec3 m_Pointing;
	float m_FOV;
};