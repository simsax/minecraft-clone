#include "Player.h"
#include "../utils/Logger.h"

Player::Player(Camera *camera) : m_Camera(camera), m_Position(m_Camera->GetCameraPosition()),
                                 m_OnGround(false),
                                 m_Inventory(std::vector(
                                         {Block::DIRT, Block::GRASS, Block::SNOW,
                                          Block::STONE, Block::WOOD, Block::DIAMOND,
                                          Block::LIGHT_RED, Block::LIGHT_GREEN, Block::LIGHT_BLUE})),
                                 m_VerticalVelocity(0.0f), m_HoldingBlock(0) {}

void Player::Move(Movement direction) {
    switch (direction) {
        case Movement::FORWARD:
            m_Camera->Forward();
            break;
        case Movement::BACKWARD:
            m_Camera->Backward();
            break;
        case Movement::LEFT:
            m_Camera->Left();
            break;
        case Movement::RIGHT:
            m_Camera->Right();
            break;
        case Movement::UP:
            if (!m_Camera->GetFlyMode() && m_OnGround)
                Jump();
            else
                m_Camera->Up();
            break;
        case Movement::DOWN:
            m_Camera->Down();
            break;
        case Movement::SPRINT:
            m_Camera->Sprint();
            break;
        case Movement::FLY:
            m_VerticalVelocity = 0.0f;
            m_Camera->ToggleFlyMode();
            break;
        default:
            break;
    }
}

bool Player::IsOnGround() {
    return m_OnGround;
}

void Player::Jump() {
    static constexpr float jumpVelocity = 10.0f;
    m_VerticalVelocity = jumpVelocity;
    m_OnGround = false;
}

bool Player::IsFlying() {
    return m_Camera->GetFlyMode();
}

float Player::GetVerticalVelocity() const {
    return m_VerticalVelocity;
}

void Player::SetVerticalVelocity(float verticalVelocity) {
    m_VerticalVelocity = verticalVelocity;
}

glm::vec3 &Player::GetPosition() {
    return m_Position;
}

glm::vec3 Player::GetSpeed() const {
    return m_Camera->GetCameraSpeed();
}

void Player::SetOnGround(bool onGround) {
    m_OnGround = onGround;
}

void Player::ResetSpeed() {
    m_Camera->Stop();
}

void Player::OnNotify(int key) {
    m_HoldingBlock = key;
    m_HoldingBlockChanged.Notify(m_HoldingBlock);
}

void Player::OnNotify(float offset) {
    m_HoldingBlock += static_cast<int>(offset);
    if (m_HoldingBlock < 0)
        m_HoldingBlock = 0;
    if (m_HoldingBlock > 8)
        m_HoldingBlock = 8;
    m_HoldingBlockChanged.Notify(m_HoldingBlock);
}

Subject<int> &Player::HoldingBlockChanged() {
    return m_HoldingBlockChanged;
}

glm::vec3 Player::GetDirection() const {
    return m_Camera->GetCameraDirection();
}

Block Player::GetHoldingBlock() const {
    return m_Inventory[m_HoldingBlock];
}
