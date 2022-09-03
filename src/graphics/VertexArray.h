#pragma once
#include "VertexBuffer.h"
#include "VertexBufferLayout.hpp"

class VertexArray
{
public:
	VertexArray();
	~VertexArray();
    VertexArray(const VertexArray&) = delete;
    VertexArray& operator=(const VertexArray&) = delete;

    void AddLayout(const VertexBufferLayout& layout, GLuint bindingIndex);
	void Bind() const;
    GLuint GetId() const;
    void Init();

private:
	GLuint m_Vao;
	uint32_t m_NumElements;
    static int8_t s_BufferCount;
    static int8_t s_CurrentlyBound;
    int8_t m_BindId;
};
