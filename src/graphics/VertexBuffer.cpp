#include "VertexBuffer.h"

VertexBuffer::VertexBuffer() :
        m_Vbo(0), m_Stride(0), m_BindingIndex(0) {
}

VertexBuffer::~VertexBuffer() {
    glDeleteBuffers(1, &m_Vbo);
}

void VertexBuffer::Init(uint32_t stride, int bindingIndex) {
    m_Stride = stride;
    m_BindingIndex = bindingIndex;
    glCreateBuffers(1, &m_Vbo);
}

void VertexBuffer::CreateStatic(GLsizeiptr size, const void *data) const {
    glNamedBufferData(m_Vbo, size, data, GL_STATIC_DRAW);
}

void VertexBuffer::CreateDynamic(GLsizeiptr size) const {
    glNamedBufferData(m_Vbo, size, nullptr, GL_DYNAMIC_DRAW);
}

void VertexBuffer::Bind(GLuint vao) const {
    glVertexArrayVertexBuffer(vao, m_BindingIndex, m_Vbo, 0, m_Stride);
}

void VertexBuffer::SendData(GLsizeiptr size, const void *data, uint32_t offset) const {
    glNamedBufferSubData(m_Vbo, offset, size, data);
}

VertexBuffer::VertexBuffer(VertexBuffer &&other) noexcept {
    this->m_Vbo = other.m_Vbo;
    this->m_Stride = other.m_Stride;
    this->m_BindingIndex = other.m_BindingIndex;
    other.m_Vbo = 0;
}

VertexBuffer &VertexBuffer::operator=(VertexBuffer &&other) noexcept {
    if (this != &other) {
        this->m_Vbo = other.m_Vbo;
        this->m_Stride = other.m_Stride;
        this->m_BindingIndex = other.m_BindingIndex;
        other.m_Vbo = 0;
    }
    return *this;
}
