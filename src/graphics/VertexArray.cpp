#include "VertexArray.h"

VertexArray::VertexArray()
    : m_NumElements(0)
{
    glGenVertexArrays(1, &m_RendererID);
};

VertexArray::~VertexArray() { glDeleteVertexArrays(1, &m_RendererID); };

void VertexArray::Bind() const { glBindVertexArray(m_RendererID); };

void VertexArray::Unbind() const { glBindVertexArray(0); };

void VertexArray::AddBuffer(const VertexBuffer& vbo, const VertexBufferLayout& layout)
{
    Bind();
    vbo.Bind();
    const auto& elements = layout.GetElements();
    unsigned int offset = 0;
    for (unsigned int i = m_NumElements; i < m_NumElements + elements.size(); i++) {
        const auto& element = elements[i - m_NumElements];
        glEnableVertexAttribArray(i);
        switch (element.type) {
        case GL_DOUBLE:
            glVertexAttribLPointer(
                i, element.count, element.type, layout.GetStride(), (const void*)offset);
            break;
        case GL_BYTE:
        case GL_UNSIGNED_BYTE:
        case GL_SHORT:
        case GL_UNSIGNED_SHORT:
        case GL_INT:
        case GL_UNSIGNED_INT:
            glVertexAttribIPointer(
                i, element.count, element.type, layout.GetStride(), (const void*)offset);
            break;
        default:
            glVertexAttribPointer(i, element.count, element.type, element.normalized,
                layout.GetStride(), (const void*)offset);
            break;
        }
        offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
    }
//    m_NumElements += elements.size();
}

void VertexArray::AddInstanceBuffer(
    const VertexBuffer& vbo, const VertexBufferLayout& layout, GLuint divisor)
{
    Bind();
    vbo.Bind();
    const auto& elements = layout.GetElements();
    unsigned int offset = 0;
    for (unsigned int i = m_NumElements; i < m_NumElements + elements.size(); i++) {
        const auto& element = elements[i - m_NumElements];
        glEnableVertexAttribArray(i);
        switch (element.type) {
        case GL_DOUBLE:
            glVertexAttribLPointer(
                i, element.count, element.type, layout.GetStride(), (const void*)offset);
            break;
        case GL_BYTE:
        case GL_UNSIGNED_BYTE:
        case GL_SHORT:
        case GL_UNSIGNED_SHORT:
        case GL_INT:
        case GL_UNSIGNED_INT:
            glVertexAttribIPointer(
                i, element.count, element.type, layout.GetStride(), (const void*)offset);
            break;
        default:
            glVertexAttribPointer(i, element.count, element.type, element.normalized,
                layout.GetStride(), (const void*)offset);
            break;
        }
        glVertexAttribDivisor(i, divisor);
        offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
    }
    m_NumElements += elements.size();
}
