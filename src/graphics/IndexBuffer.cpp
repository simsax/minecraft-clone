#include "IndexBuffer.h"
#include "GL/glew.h"

IndexBuffer::IndexBuffer(GLsizeiptr size, const void* data)
    : m_Count(0)
{
    glCreateBuffers(1, &m_Ibo);
    glNamedBufferData(m_Ibo, size, data, GL_STATIC_DRAW);
};

IndexBuffer::~IndexBuffer() { glDeleteBuffers(1, &m_Ibo); }

void IndexBuffer::Bind(GLuint vao) const { glVertexArrayElementBuffer(vao, m_Ibo); }

uint32_t IndexBuffer::GetCount() const { return m_Count; }

void IndexBuffer::SetCount(uint32_t count) { m_Count = count; }
