#include "Camera.h"
#include "Physics.h"
#include "glm/gtc/matrix_transform.hpp"

#define ZNEAR 0.1f
#define ZFAR 1000.0f
#define FOV 45.0f
#define HEIGHT 1080.0f
#define WIDTH 1920.0f

Camera::Camera(glm::vec3 position)
    : m_Frustum(ZNEAR, ZFAR, FOV, HEIGHT, WIDTH)
    , m_Proj(glm::perspective(glm::radians(FOV), WIDTH / HEIGHT, ZNEAR, ZFAR))
    , m_CameraPos(position)
    , m_CameraPreviousPos(position)
    , m_CameraFront(glm::vec3(0.0f, 0.0f, -1.0f))
    , m_CameraUp(glm::vec3(0.0f, 1.0f, 0.0f))
    , m_CameraSpeed(glm::vec3(0.0f, 0.0f, 0.0f))
    , yaw(-90.0f)
    , pitch(0.0f)
    , m_FlyMode(true)
{
}

void Camera::HandleInput(const std::array<bool, GLFW_KEY_LAST>& keyPressed)
{
    float speed = 25.0f;
    m_CameraSpeed = glm::vec3(0.0f, 0.0f, 0.0f); // the velocity vector
    if (keyPressed[GLFW_KEY_LEFT_SHIFT]) // sprint
        speed *= 2;
    if (keyPressed[GLFW_KEY_SPACE] && m_FlyMode)
        m_CameraSpeed += m_CameraUp;
    if (keyPressed[GLFW_KEY_LEFT_CONTROL] && m_FlyMode)
        m_CameraSpeed -= m_CameraUp;
    if (keyPressed[GLFW_KEY_W])
        m_CameraSpeed += glm::normalize(glm::vec3(m_CameraFront.x, 0, m_CameraFront.z));
    if (keyPressed[GLFW_KEY_S])
        m_CameraSpeed -= glm::normalize(glm::vec3(m_CameraFront.x, 0, m_CameraFront.z));
    if (keyPressed[GLFW_KEY_A])
        m_CameraSpeed -= glm::normalize(glm::cross(m_CameraFront, m_CameraUp));
    if (keyPressed[GLFW_KEY_D])
        m_CameraSpeed += glm::normalize(glm::cross(m_CameraFront, m_CameraUp));

    m_CameraSpeed *= speed;
}

void Camera::Move(float deltaTime) { m_CameraPos += m_CameraSpeed * deltaTime; }

glm::mat4 Camera::GetMVP() const
{
    return m_Proj * glm::lookAt(m_CameraPos, m_CameraPos + m_CameraFront, m_CameraUp);
}

glm::vec3* Camera::GetPlayerPosition() { return &m_CameraPos; }

glm::vec3 Camera::GetPreviousPlayerPosition() const { return m_CameraPreviousPos; }

glm::vec3 Camera::GetCameraSpeed() const { return m_CameraSpeed; }

void Camera::SetCameraSpeed(const glm::vec3& speed) { m_CameraSpeed = speed; }

glm::vec3 Camera::GetPlayerDirection() const { return m_CameraFront; }

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

void Camera::ToggleFlyMode() { m_FlyMode = !m_FlyMode; }

bool Camera::GetFlyMode() const { return m_FlyMode; }

void Camera::UpdateFrustum()
{
    m_FrustumPlanes = m_Frustum.GeneratePlanes(m_CameraPos, m_CameraFront, m_CameraUp);
}

bool Camera::IsInFrustum(const glm::vec3& point)
{
    physics::Aabb chunkAabb = physics::CreateChunkAabb(point);
    for (int i = 0; i < 6; i++) {
        if (m_FrustumPlanes[i].Distance(chunkAabb.GetPositiveVertex(m_FrustumPlanes[i].normal)) < 0)
            return false;
    }
    return true;
}