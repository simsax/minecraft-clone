#pragma once
#include "GL/glew.h"

class VertexBuffer
{
public:
	VertexBuffer(uint32_t stride, int bindingIndex);
	~VertexBuffer();

	void CreateStatic(GLsizeiptr size, const void *data) const;
	void CreateDynamic(GLsizeiptr size) const;
	void Bind(GLuint vao) const;
	void SendData(GLsizeiptr size, const void *data);

private:
	uint32_t m_Vbo;
    uint32_t m_Stride;
    int m_BindingIndex;
    static int8_t m_BufferCount;
    static int8_t m_CurrentlyBound;
    int8_t m_BindId;
};
