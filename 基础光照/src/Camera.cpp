#include "Camera.h"

void Camera::move(glm::vec3 vect)
{
	m_Pos += vect;
}

void Camera::move(GLuint key)
{
	switch (key)
	{
	case GLFW_KEY_W:
		m_Pos.x += CAMERA_MOVE_SENSITIVITY * cos(m_Angle.x);
		m_Pos.y += CAMERA_MOVE_SENSITIVITY * sin(m_Angle.x);
		break;
	case GLFW_KEY_S:
		m_Pos.x -= CAMERA_MOVE_SENSITIVITY * cos(m_Angle.x);
		m_Pos.y -= CAMERA_MOVE_SENSITIVITY * sin(m_Angle.x);
		break;
	case GLFW_KEY_A:
		m_Pos.y += CAMERA_MOVE_SENSITIVITY * cos(m_Angle.x);
		m_Pos.x -= CAMERA_MOVE_SENSITIVITY * sin(m_Angle.x);
		break;
	case GLFW_KEY_D:
		m_Pos.y -= CAMERA_MOVE_SENSITIVITY * cos(m_Angle.x);
		m_Pos.x += CAMERA_MOVE_SENSITIVITY * sin(m_Angle.x);
		break;
	case GLFW_KEY_SPACE:
		m_Pos.z += CAMERA_MOVE_SENSITIVITY;
		break;
	case GLFW_KEY_LEFT_SHIFT:
		m_Pos.z -= CAMERA_MOVE_SENSITIVITY;
		break;
	}
}

void Camera::rotate(glm::vec3 angle)
{
	m_Angle += angle * CAMERA_ROTATE_SENSITIVITY;
	if (m_Angle.y > CAMERA_PITCH_LIMIT) m_Angle.y = CAMERA_PITCH_LIMIT;
	if (m_Angle.y < -CAMERA_PITCH_LIMIT) m_Angle.y = -CAMERA_PITCH_LIMIT;
}

void Camera::changeFOV(float offset)
{
	m_FOV -= offset * CAMERA_FOV_SENSITIVITY;
	if (m_FOV > 75.0) m_FOV = 75.0;
	if (m_FOV < 25.0) m_FOV = 25.0;
}

glm::vec3 Camera::pointing() const
{
	float aX = cos(m_Angle.y) * cos(m_Angle.x);
	float aY = cos(m_Angle.y) * sin(m_Angle.x);
	float aZ = sin(m_Angle.y);
	return glm::normalize(glm::vec3(aX, aY, aZ));
}

glm::mat4 Camera::getViewMatrix()
{
	float aX = cos(m_Angle.y) * cos(m_Angle.x);
	float aY = cos(m_Angle.y) * sin(m_Angle.x);
	float aZ = sin(m_Angle.y);
	glm::vec3 lookingAt = m_Pos + glm::vec3(aX, aY, aZ);
	glm::mat4 view = glm::lookAt(m_Pos, lookingAt, VEC_UP);
	return view;
}

glm::mat4 Camera::getViewMatrix(glm::vec3 targetPos)
{
	glm::mat4 view = glm::lookAt(m_Pos, targetPos, VEC_UP);
	return view;
}