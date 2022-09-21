#pragma once

#include <optional>
#include <functional>
#include <cmath>

template<typename T>
class RayCast {
public:
    RayCast(float maxDistance, std::function<bool(const T&)> isSolid);
    std::optional<T> Cast(const T& startingPosition, const T& direction);

private:
    float m_MaxDistance;
    std::function<bool(const T&)> m_IsSolid;
};

template<typename T>
RayCast<T>::RayCast(float maxDistance, std::function<bool(const T&)> isSolid):
        m_MaxDistance(maxDistance), m_IsSolid(std::move(isSolid)) {}

template<typename T>
std::optional<T> RayCast<T>::Cast(const T& startPosition, const T& direction) {
    float Sx = std::abs(1 / direction.x);
    float Sy = std::abs(1 / direction.y);
    float Sz = std::abs(1 / direction.z);
    T currentVoxel
            = {std::floor(startPosition.x), std::floor(startPosition.y), std::floor(startPosition.z)};
    T rayLength;
    T step;

    if (direction.x < 0) {
        step.x = -1;
        rayLength.x = (startPosition.x - currentVoxel.x) * Sx;
    } else {
        step.x = 1;
        rayLength.x = (currentVoxel.x + 1 - startPosition.x) * Sx;
    }
    if (direction.y < 0) {
        step.y = -1;
        rayLength.y = (startPosition.y - currentVoxel.y) * Sy;
    } else {
        step.y = 1;
        rayLength.y = (currentVoxel.y + 1 - startPosition.y) * Sy;
    }
    if (direction.z < 0) {
        step.z = -1;
        rayLength.z = (startPosition.z - currentVoxel.z) * Sz;
    } else {
        step.z = 1;
        rayLength.z = (currentVoxel.z + 1 - startPosition.z) * Sz;
    }

    float distance = 0.0f;
    while (distance < m_MaxDistance) {
        // walk
        if (rayLength.x < rayLength.y) {
            if (rayLength.x < rayLength.z) {
                currentVoxel.x += step.x;
                distance = rayLength.x;
                rayLength.x += Sx;
            } else {
                currentVoxel.z += step.z;
                distance = rayLength.z;
                rayLength.z += Sz;
            }
        } else {
            if (rayLength.y < rayLength.z) {
                currentVoxel.y += step.y;
                distance = rayLength.y;
                rayLength.y += Sy;
            } else {
                currentVoxel.z += step.z;
                distance = rayLength.z;
                rayLength.z += Sz;
            }
        }

        if (m_IsSolid(currentVoxel))
            return currentVoxel;
    }

    return std::nullopt;
}
