#pragma once
#include "glm/glm.hpp"
#include <array>

namespace cam {

	enum class Key : unsigned char {
		NONE,
		LEFT_SHIFT,
		LEFT_CONTROL,
		SPACE,
		UP,
		LEFT,
		RIGHT,
		DOWN,
		F,
		Key_MAX = DOWN
	};

	class Camera
	{
	public:
		Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f));

		void ProcessKeyboard(const std::array<Key, static_cast<int>(cam::Key::Key_MAX) + 1>& keyPressed, float deltaTime);
		void ProcessMouse(float xoffset, float yoffset);
		void SetFlyMode(bool flyMode);
		glm::mat4 GetViewMatrix() const;
		glm::vec3* GetPlayerPosition();
		glm::vec3 GetPlayerDirection() const;
	private:
		glm::vec3 m_CameraPos;
		glm::vec3 m_CameraFront;
		glm::vec3 m_CameraUp;
		float yaw;
		float pitch;
		bool m_FlyMode;
	};

}
