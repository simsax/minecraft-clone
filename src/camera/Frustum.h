#pragma once
#include <array>

#include "glm/glm.hpp"

namespace frustum {
struct Plane {
    glm::vec3 point;
    glm::vec3 normal;

    inline float Distance(const glm::vec3& other_point)
    {
        glm::vec3 v = other_point - point;
        return glm::dot(v, normal);
    }
};

class Frustum {
public:
    Frustum(float znear, float zfar, float fov, float height, float width);
    std::array<Plane, 6> GeneratePlanes(
        const glm::vec3& cameraPos, const glm::vec3& cameraFront, const glm::vec3& cameraUp);

private:
    enum P { TOP, BOTTOM, LEFT, RIGHT, NEARP, FARP };
    float m_Znear;
    float m_Zfar;
    float m_Fov;
    float m_Height;
    float m_Width;
    float m_Ratio;
    float m_TanFOV;
};
} // namespace frustum