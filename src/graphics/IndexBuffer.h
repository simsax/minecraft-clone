#pragma once

#include "GL/glew.h"

class IndexBuffer {
public:
    IndexBuffer(GLsizeiptr size, const void* data);
    ~IndexBuffer();

    void Bind() const;
    void UnBind() const;
    uint32_t GetCount() const;
    void SetCount(uint32_t count);

private:
    uint32_t m_RendererID;
    uint32_t m_Count;
};
