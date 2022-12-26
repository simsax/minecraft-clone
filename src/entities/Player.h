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
	explicit Player(Camera* camera);
	void Move(Movement direction);
	bool IsOnGround() const;
	bool IsOnGroundCamera() const;
	bool IsFlying() const;
	bool IsMoving() const;
	bool IsRuning() const;
	void StopMoving();
	[[nodiscard]] float GetVerticalVelocity() const;
	void SetVerticalVelocity(float verticalVelocity);
	glm::vec3 GetPosition() const;
	void SetPosition(const glm::vec3& position);
	[[nodiscard]] glm::vec3 GetDirection() const;
	[[nodiscard]] glm::vec3 GetSpeed() const;
	void SetOnGround(bool onGround);
	void SetOnGroundCamera(bool onGround);
	void ResetSpeed();
	void OnNotify(int key);
	void OnNotify(float offset);
	Block GetHoldingBlock() const;
	Subject<int>& HoldingBlockChanged();
	void SetHeadBob(bool headBob);
	bool GetHeadBob() const;

private:
	void Jump();

	Camera* m_Camera;
	glm::vec3 m_Position;
	glm::vec3 m_Speed;
	bool m_OnGround;
	bool m_OnGroundCamera;
	std::vector<Block> m_Inventory;
	float m_VerticalVelocity;
	int m_HoldingBlock;
	Subject<int> m_HoldingBlockChanged;
	bool m_IsMoving;
	bool m_IsRunning;
	bool m_HeadBob;
};
