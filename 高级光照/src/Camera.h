#pragma once

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "FPSTimer.h"

const float CAMERA_ROTATE_SENSITIVITY = 0.05f;
const float CAMERA_MOVE_SENSITIVITY = 0.1f;
const float CAMERA_ROLL_SENSITIVITY = 0.05f;
const float CAMERA_FOV_SENSITIVITY = 150.0f;
const float CAMERA_PITCH_LIMIT = 88.0f;
const float CAMERA_FOV = 45.0f;
const glm::vec3 VEC_UP = glm::vec3(0.0f, 0.0f, 1.0f);

class Camera
{
public:
	Camera(glm::vec3 pos = glm::vec3(0, 0, 0), glm::vec3 angle = glm::radians(glm::vec3(90.0f, 0.0f, 0.0f)))
		:m_Pos(pos), m_Angle(angle), m_FOV(CAMERA_FOV), m_CameraUp(VEC_UP){}

	void move(glm::vec3 vect);
	void move(GLuint key);
	void roll(float angle);
	void rotate(glm::vec3 angle);
	void changeFOV(float offset);
	void lookAt(glm::vec3 pos);
	void setPos(glm::vec3 pos) { m_Pos = pos; }
	void setAngle(glm::vec3 angle) { m_Angle = glm::radians(angle); }
	void setDir(glm::vec3 dir);

	float FOV() const { return m_FOV; }
	glm::vec3 pos() const { return m_Pos; }
	glm::vec3 angle() const { return m_Angle; }
	glm::vec3 pointing() const;
	glm::mat4 getViewMatrix();
	glm::mat4 getViewMatrix(glm::vec3 targetPos);

private:
	glm::vec3 m_Pos;
	glm::vec3 m_Angle;
	glm::vec3 m_Pointing;
	glm::vec3 m_CameraUp;
	float m_FOV;
};