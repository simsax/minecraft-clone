#include "VertexArray.h"

VertexArray::VertexArray()
        : m_NumElements(0) {
    glCreateVertexArrays(1, &m_Vao);
};

VertexArray::~VertexArray() { glDeleteVertexArrays(1, &m_Vao); };

void VertexArray::Bind() const { glBindVertexArray(m_Vao); };

void VertexArray::AddLayout(const VertexBufferLayout &layout, GLuint bindingIndex) {
    const auto &elements = layout.GetElements();
    unsigned int offset = 0;
    for (unsigned int i = m_NumElements; i < m_NumElements + elements.size(); i++) {
        const auto &element = elements[i - m_NumElements];
        glEnableVertexArrayAttrib(m_Vao, i);
        switch (element.type) {
            case GL_DOUBLE:
                glVertexArrayAttribLFormat(
                        m_Vao, i, element.count, element.type, offset);
                break;
            case GL_BYTE:
            case GL_UNSIGNED_BYTE:
            case GL_SHORT:
            case GL_UNSIGNED_SHORT:
            case GL_INT:
            case GL_UNSIGNED_INT:
                glVertexArrayAttribIFormat(
                        m_Vao, i, element.count, element.type, offset);
                break;
            default:
                glVertexArrayAttribFormat(m_Vao, i, element.count, element.type,
                                          element.normalized,
                                          offset);
                break;
        }
        glVertexArrayAttribBinding(m_Vao, i, bindingIndex);
        offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
    }
}

GLuint VertexArray::GetId() const {
    return m_Vao;
}
