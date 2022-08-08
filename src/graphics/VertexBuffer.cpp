#include "VertexBuffer.h"

int8_t VertexBuffer::m_BufferCount = 0;
int8_t VertexBuffer::m_CurrentlyBound = -1;

VertexBuffer::VertexBuffer(uint32_t stride, int bindingIndex):
 m_Stride(stride), m_BindingIndex(bindingIndex)
{
    m_BindId = m_BufferCount;
    m_BufferCount++;
	glCreateBuffers(1, &m_Vbo);
}

VertexBuffer::~VertexBuffer()
{
	glDeleteBuffers(1, &m_Vbo);
}

void VertexBuffer::CreateStatic(GLsizeiptr size, const void *data) const
{
    glNamedBufferData(m_Vbo, size, data, GL_STATIC_DRAW);
}

void VertexBuffer::CreateDynamic(GLsizeiptr size) const
{
    glNamedBufferData(m_Vbo, size, nullptr, GL_DYNAMIC_DRAW);
}

void VertexBuffer::Bind(GLuint vao) const {
    if (m_CurrentlyBound != m_BindId) {
        glVertexArrayVertexBuffer(vao, m_BindingIndex, m_Vbo, 0, m_Stride);
        m_CurrentlyBound = m_BindId;
    }
}

void VertexBuffer::SendData(GLsizeiptr size, const void *data)
{
	glNamedBufferSubData(m_Vbo, 0, size, data);
}
