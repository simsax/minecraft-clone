#include "Renderer.h"
#include "Config.h"

void Renderer::Clear() const
{
	glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::SetMVP(const glm::mat4& mvp)
{
	m_MVP = mvp;
}

void Renderer::Init()
{
	m_Shader = std::make_unique<Shader>(std::string(SOURCE_DIR) + "/res/shaders/Basic.shader");
	m_Shader->Bind();

	m_Texture = std::make_unique<Texture>(std::string(SOURCE_DIR) + "/res/textures/terrain.png");
	m_Texture->Bind(0);
	m_Shader->SetUniform1i("u_Texture", 0);
}

void Renderer::Draw(const VertexArray& vao, const IndexBuffer& ibo, GLenum type) const
{
	m_Shader->SetUniformMat4f("u_MVP", m_MVP);
	m_Shader->Bind();
	vao.Bind();
	ibo.Bind();
	// primitive, num of indices, type of indices
	glDrawElements(GL_TRIANGLES, ibo.GetCount(), type, nullptr);
}

void Renderer::DrawInstanced(const VertexArray& vao, const IndexBuffer& ibo, GLenum type, GLsizei num_instances) const
{
	m_Shader->SetUniformMat4f("u_MVP", m_MVP);
	m_Shader->Bind();
	vao.Bind();
	ibo.Bind();
	glDrawElementsInstanced(GL_TRIANGLES, ibo.GetCount(), type, nullptr, num_instances);
}
