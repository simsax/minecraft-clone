#pragma once

#include "GL/glew.h"
#include <memory>
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Texture.h"
#include "Shader.h"
#include "ChunkRenderer.h"
#include "GuiRenderer.h"
#include "SkyRenderer.h"
#include "QuadRenderer.h"

class Renderer {
public:
	void Init(int width, int height);
	void Resize(int width, int height);
	static void Clear(const glm::vec4& skyColor);
	void SetViewMatrix(const glm::mat4& view);
	void ImGuiRender();

	ChunkRenderer chunkRenderer;
	GuiRenderer guiRenderer;
	QuadRenderer quadRenderer;
	SkyRenderer skyRenderer;

private:
	glm::mat4 m_View;
	glm::mat4 m_PersProj;
	glm::mat4 m_OrthoProj;
};
