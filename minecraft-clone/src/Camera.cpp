#include "Camera.h"
#include "glm/gtc/matrix_transform.hpp"

namespace cam {

	Camera::Camera(glm::vec3 position) :
	m_CameraPos(std::move(position)),
	m_CameraFront(glm::vec3(0.0f, 0.0f, -1.0f)),
	m_CameraUp(glm::vec3(0.0f, 1.0f, 0.0f)),
	yaw(-90.0f), // point towards negative z-axis
	pitch(0.0f),
	m_FlyMode(true)
	{}

	void Camera::ProcessKeyboard(const std::array<Key, static_cast<int>(cam::Key::Key_MAX) + 1>& keyPressed, float deltaTime) {
		float cameraSpeed = 5.0f * deltaTime;

		for (const auto& key : keyPressed) {
			if (key != Key::NONE) {
				if (key == Key::LEFT_SHIFT) // sprint
					cameraSpeed *= 2;
				if (key == Key::SPACE && m_FlyMode)
					m_CameraPos += cameraSpeed * m_CameraUp;
				if (key == Key::LEFT_CONTROL && m_FlyMode)
					m_CameraPos -= cameraSpeed * m_CameraUp;
				if (key == Key::UP)
					m_CameraPos += cameraSpeed * glm::normalize(glm::vec3(m_CameraFront.x, 0, m_CameraFront.z));
				if (key == Key::DOWN)
					m_CameraPos -= cameraSpeed * glm::normalize(glm::vec3(m_CameraFront.x, 0, m_CameraFront.z));
				if (key == Key::LEFT)
					m_CameraPos -= cameraSpeed * glm::normalize(glm::cross(m_CameraFront, m_CameraUp));
				if (key == Key::RIGHT)
					m_CameraPos += cameraSpeed * glm::normalize(glm::cross(m_CameraFront, m_CameraUp));
			}
		}
	}

	glm::mat4 Camera::GetViewMatrix() const
	{
		return glm::lookAt(m_CameraPos, m_CameraPos + m_CameraFront, m_CameraUp);
	}

	glm::vec3* Camera::GetPlayerPosition()
	{
		return &m_CameraPos;
	}

	glm::vec3 Camera::GetPlayerDirection() const
	{
		return m_CameraFront;
	}

	void Camera::ProcessMouse(float xoffset, float yoffset)
	{
		yaw += xoffset;
		pitch += yoffset;

		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;

		glm::vec3 direction;
		direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		direction.y = sin(glm::radians(pitch));
		direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		m_CameraFront = glm::normalize(direction);
	}

	void Camera::SetFlyMode(bool flyMode)
	{
		m_FlyMode = flyMode;
	}

}