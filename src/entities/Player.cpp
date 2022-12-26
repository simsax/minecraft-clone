#include "Player.h"
#include "../utils/Logger.h"

Player::Player(Camera* camera) : m_Camera(camera), m_Speed(glm::vec3(0)),
m_Position(m_Camera->GetPosition()), m_OnGround(false), m_OnGroundCamera(false),
m_Inventory(std::vector(
	{ Block::DIRT, Block::GRASS, Block::SNOW,
	 Block::STONE, Block::WOOD, Block::DIAMOND,
	 Block::LIGHT_RED, Block::LIGHT_GREEN, Block::LIGHT_BLUE })),
	m_VerticalVelocity(0.0f), m_HoldingBlock(0), m_IsMoving(false), m_IsRunning(false),
	m_HeadBob(false) {}

void Player::Move(Movement direction) {
	m_IsMoving = true;
	switch (direction) {
	case Movement::FORWARD:
		m_Speed = m_Camera->Forward();
		break;
	case Movement::BACKWARD:
		m_Speed = m_Camera->Backward();
		break;
	case Movement::LEFT:
		m_Speed = m_Camera->Left();
		break;
	case Movement::RIGHT:
		m_Speed = m_Camera->Right();
		break;
	case Movement::UP:
		if (!m_Camera->GetFlyMode() && m_OnGround)
			Jump();
		else
			m_Speed = m_Camera->Up();
		break;
	case Movement::DOWN:
		m_Speed = m_Camera->Down();
		break;
	case Movement::SPRINT:
		m_Speed = m_Camera->Sprint();
		m_IsRunning = true;
		break;
	case Movement::FLY:
		m_VerticalVelocity = 0.0f;
		m_Camera->ToggleFlyMode();
		break;
	default:
		break;
	}
}

bool Player::IsOnGround() const {
	return m_OnGround;
}

bool Player::IsOnGroundCamera() const {
	return m_OnGroundCamera;
}

void Player::SetHeadBob(bool headbob) {
	m_HeadBob = headbob;
}

bool Player::GetHeadBob() const {
	return m_HeadBob;
}

void Player::Jump() {
	static constexpr float jumpVelocity = 10.0f;
	m_VerticalVelocity = jumpVelocity;
	m_OnGround = false;
}

bool Player::IsFlying() const {
	return m_Camera->GetFlyMode();
}

bool Player::IsMoving() const {
	return m_IsMoving;
}

bool Player::IsRuning() const
{
	return m_IsRunning;
}

void Player::StopMoving() {
	m_IsMoving = false;
	m_IsRunning = false;
}

float Player::GetVerticalVelocity() const {
	return m_VerticalVelocity;
}

void Player::SetVerticalVelocity(float verticalVelocity) {
	m_VerticalVelocity = verticalVelocity;
}

glm::vec3 Player::GetPosition() const {
	return m_Position;
}

void Player::SetPosition(const glm::vec3& position) {
	m_Position = position;
	m_Camera->SetPosition(position);
}

glm::vec3 Player::GetSpeed() const {
	return m_Speed;
}

void Player::SetOnGround(bool onGround) {
	m_OnGround = onGround;
}

void Player::SetOnGroundCamera(bool onGround) {
	m_OnGroundCamera = onGround;
}

void Player::ResetSpeed() {
	m_Speed = m_Camera->Stop();
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

Subject<int>& Player::HoldingBlockChanged() {
	return m_HoldingBlockChanged;
}

glm::vec3 Player::GetDirection() const {
	return m_Camera->GetDirection();
}

Block Player::GetHoldingBlock() const {
	return m_Inventory[m_HoldingBlock];
}
