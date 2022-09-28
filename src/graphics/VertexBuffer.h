#pragma once
#include "GL/glew.h"
#include <cstdint>

class VertexBuffer
{
public:
    VertexBuffer();
	~VertexBuffer();
    VertexBuffer(const VertexBuffer&) = delete;
    VertexBuffer& operator=(const VertexBuffer&) = delete;
    VertexBuffer(VertexBuffer&& other) noexcept;
    VertexBuffer& operator=(VertexBuffer&& other) noexcept;

    void Init(uint32_t stride, int bindingIndex);
	void CreateStatic(GLsizeiptr size, const void *data) const;
	void CreateDynamic(GLsizeiptr size) const;
	void Bind(GLuint vao) const;
	void SendData(GLsizeiptr size, const void *data, uint32_t offset) const;

private:
	uint32_t m_Vbo;
    uint32_t m_Stride;
    int m_BindingIndex;
};
