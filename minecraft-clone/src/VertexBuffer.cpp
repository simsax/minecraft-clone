#include "VertexBuffer.h"

VertexBuffer::VertexBuffer(unsigned int size, const void* data) {
	GLCall(glGenBuffers(1, &m_RendererID)); //Create the VBO on the GPU. Create the buffer and returns the id (since it is void it writes the id in the variable)
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID)); // Bind our VBO to the GL_ARRAY_BUFFER 'variable' on the GPU
	GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW)); //Here we do an operation on the currently bound GL_ARRAY_BUFFER VBO, which is our vbo. This function allocates the memory on the GPU with the size and type of data we passed
}

VertexBuffer::~VertexBuffer() {
	GLCall(glDeleteBuffers(1, &m_RendererID));
}

void VertexBuffer::Bind() const {
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
}

void VertexBuffer::UnBind() const {
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}


void VertexBuffer::SendData(unsigned int size, const void* data)
{
	this->Bind();
	GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, size, data));
}
