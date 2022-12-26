#pragma once
#include "Frustum.h"
#include "glm/glm.hpp"
#include <array>

class Camera {
public:
	Camera(const glm::vec3& position, int width, int height);
	glm::vec3 Forward();
	glm::vec3 Backward();
	glm::vec3 Left();
	glm::vec3 Right();
	glm::vec3 Up();
	glm::vec3 Down();
	glm::vec3 Sprint();
	void Move();
	glm::vec3 Stop();
	void Watch(float xoffset, float yoffset);
	void ToggleFlyMode();
	bool GetFlyMode() const;
	glm::mat4 GetViewMatrix() const;
	glm::vec3 GetPosition() const;
	void SetPosition(const glm::vec3& position);
	glm::vec3 GetDirection() const;
	glm::vec3 GetSpeed() const;
	glm::vec3 GetFrontVector() const;
	void OffsetHorizontalPosition(float val);
	void UpdateFrustum();
	bool IsInFrustum(const glm::vec3& point);
	void Resize(int width, int height);


private:
	frustum::Frustum m_Frustum;
	std::array<frustum::Plane, 6> m_FrustumPlanes;
	glm::vec3 m_CameraPos;
	glm::vec3 m_CameraFront;
	glm::vec3 m_CameraUp;
	glm::vec3 m_CameraSpeed;
	float yaw;
	float pitch;
	bool m_FlyMode;
	float m_Speed;
};
