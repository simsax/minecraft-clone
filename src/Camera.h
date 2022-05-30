#pragma once
#include "Frustum.h"
#include "Keycodes.h"
#include "glm/glm.hpp"
#include <array>

class Camera {
public:
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f));

    void HandleInput(const std::array<bool, GLFW_KEY_LAST>& keyPressed);
    void ProcessMouse(float xoffset, float yoffset);
    void ToggleFlyMode();
    bool GetFlyMode() const;
    void SetCameraSpeed(glm::vec3 speed);
    void Move(float deltaTime);
    glm::mat4 GetMVP() const;
    glm::vec3* GetPlayerPosition();
    glm::vec3 GetPreviousPlayerPosition() const;
    glm::vec3 GetPlayerDirection() const;
    glm::vec3 GetCameraSpeed() const;
    void UpdateFrustum();
    bool IsInFrustum(const glm::vec3& point);

private:
    frustum::Frustum m_Frustum;
    std::array<frustum::Plane, 6> m_FrustumPlanes;
    glm::mat4 m_Proj;
    glm::vec3 m_CameraPos;
    glm::vec3 m_CameraPreviousPos;
    glm::vec3 m_CameraFront;
    glm::vec3 m_CameraUp;
    glm::vec3 m_CameraSpeed;
    float yaw;
    float pitch;
    bool m_FlyMode;
};
