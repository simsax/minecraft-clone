#pragma once

#include "glm/glm.hpp"
#include "../camera/Camera.h"
#include "../utils/Subject.hpp"
#include "../utils/Items.h"

enum class Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN,
    SPRINT,
    FLY
};

class Player {
public:
    Player(Camera* camera);
    void Move(Movement direction);
    bool IsOnGround();
    bool IsFlying();
    float GetVerticalVelocity() const;
    void SetVerticalVelocity(float verticalVelocity);
    glm::vec3& GetPosition();
    glm::vec3 GetDirection() const;
    glm::vec3 GetSpeed() const;
    void SetOnGround(bool onGround);
    void ResetSpeed();
    void OnNotify(int key);
    void OnNotify(float offset);
    Block GetHoldingBlock() const;
    Subject<int>& HoldingBlockChanged();

private:
    void Jump();

    Camera* m_Camera;
    glm::vec3& m_Position;
    bool m_OnGround;
    std::vector<Block> m_Inventory;
    float m_VerticalVelocity;
    int m_HoldingBlock;
    Subject<int> m_HoldingBlockChanged;
};
