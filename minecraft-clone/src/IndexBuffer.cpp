#pragma once

#include "IndexBuffer.h"
#include "GL/glew.h"

IndexBuffer::IndexBuffer(GLsizeiptr size, const void* data) : m_Count(0) {
	glGenBuffers(1, &m_RendererID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW); //Here we do an operation on the currently bound GL_ELEMENT_ARRAY_BUFFER, which is our ibo. This function allocates the memory on the GPU with the size we passed, and also with the data we passed
};

IndexBuffer::~IndexBuffer() {
	glDeleteBuffers(1, &m_RendererID);
}

void IndexBuffer::Bind() const {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
}

void IndexBuffer::UnBind() const {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

unsigned int IndexBuffer::GetCount() const
{
	return m_Count;
}

void IndexBuffer::SetCount(unsigned int count)
{
	m_Count = count;
}