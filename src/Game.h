#pragma once

#include "camera/Camera.h"
#include "world/ChunkManager.h"
#include "gui/GuiManager.h"
#include "graphics/Window.h"
#include "utils/InputHandler.h"
#include "entities/Player.h"
#include "utils/RayCast.hpp"
#include "graphics/ChunkRenderer.h"
#include "graphics/SkyRenderer.h"
#include "sky/SkyEntities.h"

class Game {
public:
	Game();
	void Run();

private:
	void Init();
	void OnUpdate(float deltaTime);
	void OnRender();
	void CheckRayCast();
	void UpdateChunks();
	void BindCommands();
	void UpdateTime(float deltaTime);
	void Resize(int width, int height);
	void Pause();
	void ShowGui();
	static void UpdateFPS(uint32_t numFrames);

	int m_Width;
	int m_Height;
	bool m_ShowGui;
	bool m_Pause;

	InputHandler m_InputHandler;
	Window m_Window;
	Camera m_Camera;
	Renderer m_Renderer;
	Player m_Player;
	ChunkManager m_ChunkManager;
	GuiManager m_GuiManager;
	RayCast<glm::vec3> m_RayCast;
	SkyEntities m_SkyEntities;
};
