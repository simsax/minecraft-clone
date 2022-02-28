#include "VertexBuffer.h"

VertexBuffer::VertexBuffer() {
	glGenBuffers(1, &m_RendererID); //Create the VBO on the GPU. Create the buffer and returns the id (since it is void it writes the id in the variable)
}

VertexBuffer::~VertexBuffer() {
	glDeleteBuffers(1, &m_RendererID);
}

void VertexBuffer::CreateStatic(GLsizeiptr size, const void* data) const
{
	glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

void VertexBuffer::CreateDynamic(GLsizeiptr size) const
{
	glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
	glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
}

void VertexBuffer::Bind() const {
	glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
}

void VertexBuffer::UnBind() const {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBuffer::SendData(GLsizeiptr size, const void* data)
{
	this->Bind();
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
}
