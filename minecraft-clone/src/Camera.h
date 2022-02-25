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
		DOWN
	};

	class Camera
	{
	public:
		Camera(const glm::vec3& position = glm::vec3(0.0f, 0.0f, 0.0f));

		void ProcessKeyboard(const std::array<Key, 7>& keyPressed, float deltaTime);
		void ProcessMouse(float xoffset, float yoffset);
		glm::mat4 GetViewMatrix() const;
	private:
		glm::vec3 m_CameraPos;
		glm::vec3 m_CameraUp;
		glm::vec3 m_CameraFront;
		float yaw;
		float pitch;

	};

}
