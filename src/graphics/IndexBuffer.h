#pragma once

#include <cstdint>
#include "GL/glew.h"

class IndexBuffer {
public:
    IndexBuffer();
    ~IndexBuffer();
    IndexBuffer(const IndexBuffer&) = delete;
    IndexBuffer& operator=(const IndexBuffer&) = delete;

    void Init(GLsizeiptr size, const void* data);
    void Bind(GLuint vao) const;
    uint32_t GetCount() const;
    void SetCount(uint32_t count);

private:
    uint32_t m_Ibo;
    uint32_t m_Count;
    static int8_t m_BufferCount;
    static int8_t m_CurrentlyBound;
    int8_t m_BindId;
};
