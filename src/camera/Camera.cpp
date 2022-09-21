#include "Camera.h"
#include "../utils/Physics.h"
#include "glm/gtc/matrix_transform.hpp"
#include "Constants.h"


Camera::Camera(const glm::vec3& position, int width, int height)
    : m_Frustum(ZNEAR, ZFAR, glm::radians(FOV), width, height)
    , m_CameraPos(position)
    , m_CameraFront(glm::vec3(0.0f, 0.0f, -1.0f))
    , m_CameraUp(glm::vec3(0.0f, 1.0f, 0.0f))
    , m_CameraSpeed(glm::vec3(0.0f, 0.0f, 0.0f))
    , yaw(-90.0f)
    , pitch(0.0f)
    , m_FlyMode(true)
    , m_Speed(25.0f)
{
}

void Camera::Forward() {
    m_CameraSpeed += glm::normalize(glm::vec3(m_CameraFront.x, 0, m_CameraFront.z)) * m_Speed;
}

void Camera::Backward() {
    m_CameraSpeed -= glm::normalize(glm::vec3(m_CameraFront.x, 0, m_CameraFront.z)) * m_Speed;
}

void Camera::Right() {
    m_CameraSpeed += glm::normalize(glm::cross(m_CameraFront, m_CameraUp)) * m_Speed;
}

void Camera::Left() {
    m_CameraSpeed -= glm::normalize(glm::cross(m_CameraFront, m_CameraUp)) * m_Speed;
}

void Camera::Up() {
    if (m_FlyMode)
        m_CameraSpeed += m_CameraUp * m_Speed;
}

void Camera::Down() {
    if (m_FlyMode)
        m_CameraSpeed -= m_CameraUp * m_Speed;
}

void Camera::Sprint() {
   m_CameraSpeed *= 2;
}

void Camera::Move() {
    m_CameraSpeed *= m_Speed;
}

void Camera::Stop() {
    m_CameraSpeed = glm::vec3(0.0f);
}

glm::mat4 Camera::GetViewMatrix() const
{
    return glm::lookAt(m_CameraPos, m_CameraPos + m_CameraFront, m_CameraUp);
}

glm::vec3& Camera::GetCameraPosition() { return m_CameraPos; }

glm::vec3 Camera::GetCameraSpeed() { return m_CameraSpeed; }

glm::vec3 Camera::GetCameraDirection() const { return m_CameraFront; }

void Camera::Watch(float xoffset, float yoffset)
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

void Camera::ToggleFlyMode() {
    m_FlyMode = !m_FlyMode;
    if (m_FlyMode)
        m_Speed = 25.0f;
    else
        m_Speed = 5.0f;
}

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

void Camera::Resize(int width, int height) {
    m_Frustum.Resize(width, height);
}

glm::vec3 Camera::GetFrontVector() const {
    return m_CameraFront;
}
