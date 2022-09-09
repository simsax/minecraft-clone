#pragma once
#include "Frustum.h"
#include "../utils/Keycodes.h"
#include "glm/glm.hpp"
#include <array>

class Camera {
public:
    Camera(const glm::vec3& position, int width, int height);

    void HandleInput(const std::array<bool, GLFW_KEY_LAST>& keyPressed);
    void ProcessMouse(float xoffset, float yoffset);
    void ToggleFlyMode();
    bool GetFlyMode() const;
    void Move(float deltaTime);
    glm::mat4 GetViewMatrix() const;
    glm::vec3& GetPlayerPosition();
    glm::vec3 GetPlayerDirection() const;
    glm::vec3 GetCameraSpeed();
    glm::vec3 GetFrontVector() const;
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
