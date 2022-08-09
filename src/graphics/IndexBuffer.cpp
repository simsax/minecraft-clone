#include "IndexBuffer.h"
#include "GL/glew.h"

int8_t IndexBuffer::m_BufferCount = 0;
int8_t IndexBuffer::m_CurrentlyBound = -1;

void IndexBuffer::Init(GLsizeiptr size, const void *data)
{
    glCreateBuffers(1, &m_Ibo);
    glNamedBufferData(m_Ibo, size, data, GL_STATIC_DRAW);
};

IndexBuffer::~IndexBuffer() { glDeleteBuffers(1, &m_Ibo); }

void IndexBuffer::Bind(GLuint vao) const {

    if (m_CurrentlyBound != m_BindId) {
        glVertexArrayElementBuffer(vao, m_Ibo);
        m_CurrentlyBound = m_BindId;
    }
}

uint32_t IndexBuffer::GetCount() const { return m_Count; }

void IndexBuffer::SetCount(uint32_t count) { m_Count = count; }

IndexBuffer::IndexBuffer(): m_Ibo(0), m_Count(0) {
    m_BindId = m_BufferCount;
    m_BufferCount++;
}
