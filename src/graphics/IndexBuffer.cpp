#include "IndexBuffer.h"
#include "GL/glew.h"

IndexBuffer::IndexBuffer(GLsizeiptr size, const void* data)
    : m_Count(0)
{
    glGenBuffers(1, &m_RendererID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
};

IndexBuffer::~IndexBuffer() { glDeleteBuffers(1, &m_RendererID); }

void IndexBuffer::Bind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID); }

void IndexBuffer::UnBind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }

uint32_t IndexBuffer::GetCount() const { return m_Count; }

void IndexBuffer::SetCount(uint32_t count) { m_Count = count; }
