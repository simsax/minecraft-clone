#include "Frustum.h"

namespace frustum {
    Frustum::Frustum(float znear, float zfar, float fov, int width, int height)
            : m_Znear(znear), m_Zfar(zfar), m_Fov(fov), m_Width(width), m_Height(height),
              m_TanFOV(std::tan(fov / 2)),
              m_Ratio(static_cast<float>(width) / static_cast<float>(height)) {
    }

    std::array<Plane, 6> Frustum::GeneratePlanes(
            const glm::vec3 &cameraPos, const glm::vec3 &cameraFront, const glm::vec3 &cameraUp) {
        std::array<Plane, 6> frustumPlanes;
        glm::vec3 rightV = glm::normalize(glm::cross(cameraFront, cameraUp));
        glm::vec3 upV = glm::normalize(glm::cross(rightV, cameraFront));

        glm::vec3 nc = cameraPos + cameraFront * m_Znear;
        frustumPlanes[P::NEARP] = {nc, cameraFront};
        glm::vec3 fc = cameraPos + cameraFront * m_Zfar;
        frustumPlanes[P::FARP] = {fc, -cameraFront};

        float nh = m_Znear * m_TanFOV;
        glm::vec3 aux = glm::normalize((nc + upV * nh) - cameraPos);
        glm::vec3 normal = glm::cross(aux, rightV);
        frustumPlanes[P::TOP] = {nc + upV * nh, normal};
        aux = glm::normalize((nc - upV * nh) - cameraPos);
        normal = glm::cross(rightV, aux);
        frustumPlanes[P::BOTTOM] = {nc - upV * nh, normal};

        float nw = nh * m_Ratio;
        aux = glm::normalize((nc - rightV * nw) - cameraPos);
        normal = glm::cross(aux, upV);
        frustumPlanes[P::LEFT] = {nc - rightV * nw, normal};
        aux = glm::normalize((nc + rightV * nw) - cameraPos);
        normal = glm::cross(upV, aux);
        frustumPlanes[P::RIGHT] = {nc + rightV * nw, normal};
        return frustumPlanes;
    }

    void Frustum::Resize(int width, int height) {
        m_Width = width;
        m_Height = height;
        m_Ratio = static_cast<float>(m_Width) / static_cast<float>(m_Height);
    }
} // namespace frustum
