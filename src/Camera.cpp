#include "Camera.h"
#include "glm/gtc/matrix_transform.hpp"

namespace cam {

	Camera::Camera(glm::vec3 position) :
	m_CameraPos(position),
    m_CameraPreviousPos(position),
	m_CameraFront(glm::vec3(0.0f, 0.0f, -1.0f)),
	m_CameraUp(glm::vec3(0.0f, 1.0f, 0.0f)),
    m_CameraSpeed(glm::vec3(0.0f, 0.0f, 0.0f)),
	yaw(-90.0f), // point towards negative z-axis
	pitch(0.0f),
	m_FlyMode(true)
	{}

	void Camera::ProcessKeyboard(const std::array<Key, static_cast<int>(Key::Key_MAX) + 1>& keyPressed) {
        m_CameraSpeed = glm::vec3(0.0f, 0.0f, 0.0f); // the velocity vector
        float speed = 25.0f;
		for (const auto& key : keyPressed) {
			if (key != Key::NONE) {
				if (key == Key::LEFT_SHIFT) // sprint
					speed *= 2;
				if (key == Key::SPACE && m_FlyMode)
					m_CameraSpeed += m_CameraUp;
				if (key == Key::LEFT_CONTROL && m_FlyMode)
					m_CameraSpeed -= m_CameraUp;
				if (key == Key::UP)
					m_CameraSpeed += glm::normalize(glm::vec3(m_CameraFront.x, 0, m_CameraFront.z));
				if (key == Key::DOWN)
					m_CameraSpeed -= glm::normalize(glm::vec3(m_CameraFront.x, 0, m_CameraFront.z));
				if (key == Key::LEFT)
					m_CameraSpeed -= glm::normalize(glm::cross(m_CameraFront, m_CameraUp));
				if (key == Key::RIGHT)
					m_CameraSpeed += glm::normalize(glm::cross(m_CameraFront, m_CameraUp));
			}
		}
        m_CameraSpeed *= speed;
	}

    void Camera::Move(float deltaTime) {
        m_CameraPos += m_CameraSpeed * deltaTime;
    }

	glm::mat4 Camera::GetViewMatrix() const
	{
		return glm::lookAt(m_CameraPos, m_CameraPos + m_CameraFront, m_CameraUp);
	}

	glm::vec3* Camera::GetPlayerPosition()
	{
		return &m_CameraPos;
	}

    glm::vec3 Camera::GetPreviousPlayerPosition() const
    {
        return m_CameraPreviousPos;
    }

    glm::vec3 Camera::GetCameraSpeed() const {
        return m_CameraSpeed;
    }

    void Camera::SetCameraSpeed(glm::vec3 speed) {
        m_CameraSpeed = speed;
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