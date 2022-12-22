#include <cstdint>
#include "QuadRenderer.h"
#include "glm/gtx/transform.hpp"
#include "Config.h"
#include <array>
#include "../utils/Logger.h"

static constexpr std::array<float, 12> positions = {
		0.0f, 0.0f, 1.0f, // bottom left
		0.0f, 0.0f, 0.0f, // top left
		1.0f, 0.0f, 1.0f, // bottom right
		1.0f, 0.0f, 0.0f // top right
};

void QuadRenderer::Init(glm::mat4* proj, glm::mat4* view)
{
	m_Proj = proj;
	m_View = view;

	m_Shader.Init(std::string(SOURCE_DIR) + "/res/shaders/shader_quad.vert",
		std::string(SOURCE_DIR) + "/res/shaders/shader_quad.frag");
	m_Shader.Bind();
	m_Shader.SetUniform1i("u_Texture", 0);

	m_CloudsShader.Init(std::string(SOURCE_DIR) + "/res/shaders/shader_quad.vert",
		std::string(SOURCE_DIR) + "/res/shaders/shader_clouds.frag");
	m_CloudsShader.Bind();
	m_CloudsShader.SetUniform1i("u_Texture", 0);

	m_StarsShader.Init(std::string(SOURCE_DIR) + "/res/shaders/shader_stars.vert",
		std::string(SOURCE_DIR) + "/res/shaders/shader_stars.frag");
	m_StarsShader.Bind();
	m_StarsShader.SetUniform1i("u_Texture", 0);

	m_VertexLayout.Push<float>(3); // texture coordinates are the same as the position ones
	m_VBO.Init(m_VertexLayout.GetStride(), 0);
	m_VBO.CreateStatic(positions.size() * sizeof(float), positions.data());
	m_VAO.Init();
	m_VBO.Bind(m_VAO.GetId());
	m_VAO.AddLayout(m_VertexLayout, 0);
}

void QuadRenderer::Render(const Texture& texture, const glm::vec2& position, const glm::vec2& scale)
{
	glDisable(GL_DEPTH_TEST);
	glDepthMask(0);
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(
		model, glm::vec3(position.x - scale.x / 2.0f, position.y - scale.y / 2.0f, 0.0f));
	model = glm::scale(model, glm::vec3(scale.x, scale.y, 1.0f));
	glm::mat4 mvp = *m_Proj * model;
	m_Shader.Bind();
	m_Shader.SetUniformMat4f("u_MVP", mvp);
	texture.Bind(0);
	m_VAO.Bind();
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(1);
}


void QuadRenderer::Render(const Texture& texture, const glm::mat4& model) {
	glm::mat4 mvp = *m_Proj * *m_View * model;
	m_Shader.Bind();
	m_Shader.SetUniformMat4f("u_MVP", mvp);
	texture.Bind(0);
	m_VAO.Bind();
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void QuadRenderer::RenderStars(const Texture& texture, VertexArray& vao, const glm::mat4& model,
	uint32_t count, float alpha) {
	glm::mat4 vp = *m_Proj * *m_View;
	m_StarsShader.Bind();
	m_StarsShader.SetUniformMat4f("u_M", model);
	m_StarsShader.SetUniformMat4f("u_VP", vp);
	m_StarsShader.SetUniformMat4f("u_V", *m_View);
	m_StarsShader.SetUniform1f("u_Alpha", alpha);
	texture.Bind(0);
	vao.Bind();
	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, count);
}

void QuadRenderer::RenderClouds(const Texture& texture, const glm::mat4& model, float time, float alpha) {
	glm::mat4 mvp = *m_Proj * *m_View * model;
	m_CloudsShader.Bind();
	m_CloudsShader.SetUniformMat4f("u_MVP", mvp);
	m_CloudsShader.SetUniform1f("u_Time", time);
	m_CloudsShader.SetUniform1f("u_Color", alpha);
	texture.Bind(0);
	m_VAO.Bind();
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
