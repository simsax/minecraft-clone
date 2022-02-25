#include "Renderer.h"

void Renderer::Clear() const
{
	GLCall(glClear(GL_COLOR_BUFFER_BIT));
}

void Renderer::Draw(const VertexArray& vao, const IndexBuffer& ibo, GLenum type, const Shader& shader) const
{
	shader.Bind();
	vao.Bind();
	ibo.Bind();
	// primitive, num of indices, type of indices
	GLCall(glDrawElements(GL_TRIANGLES, ibo.GetCount(), type, nullptr));
}
