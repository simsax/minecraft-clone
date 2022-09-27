#pragma once

#include "GL/glew.h"
#include <vector>

struct VertexBufferElement {
    VertexBufferElement(GLenum type, GLint count, GLboolean normalized)
            : type(type), count(count), normalized(normalized) {};

    static unsigned int GetSizeOfType(GLenum type) {
        switch (type) {
            case GL_DOUBLE:
                return 8;
            case GL_FLOAT:
            case GL_UNSIGNED_INT:
            case GL_INT:
                return 4;
            case GL_SHORT:
            case GL_UNSIGNED_SHORT:
                return 2;
            case GL_BYTE:
            case GL_UNSIGNED_BYTE:
                return 1;
            default:
                return 0;
        }
    }

    GLenum type;
    GLint count;
    GLboolean normalized;
};

class VertexBufferLayout {
public:
    VertexBufferLayout()
            : m_Stride(0) {};

    template<typename T>
    void Push(GLint count) {
        // static_assert(false);
    }

    inline const std::vector<VertexBufferElement> GetElements() const { return m_Elements; };

    inline uint32_t GetStride() const { return m_Stride; }

private:
    std::vector<VertexBufferElement> m_Elements;
    uint32_t m_Stride;
};

template<>
inline void VertexBufferLayout::Push<float>(GLint count) {
    m_Elements.emplace_back(GL_FLOAT, count, GL_FALSE);
    m_Stride += count * VertexBufferElement::GetSizeOfType(GL_FLOAT);
}

template<>
inline void VertexBufferLayout::Push<uint32_t>(GLint count) {
    m_Elements.emplace_back(GL_UNSIGNED_INT, count, GL_FALSE);
    m_Stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_INT);
}

template<>
inline void VertexBufferLayout::Push<uint16_t>(GLint count) {
    m_Elements.emplace_back(GL_UNSIGNED_SHORT, count, GL_FALSE);
    m_Stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_SHORT);
}

template<>
inline void VertexBufferLayout::Push<uint8_t>(GLint count) {
    m_Elements.emplace_back(GL_UNSIGNED_BYTE, count, GL_FALSE);
    m_Stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_BYTE);
}
