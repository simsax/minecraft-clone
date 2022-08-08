#pragma once
#include "VertexBuffer.h"
#include "VertexBufferLayout.hpp"

class VertexArray
{
public:
	VertexArray();
	~VertexArray();

    void AddLayout(const VertexBufferLayout& layout, GLuint bindingIndex);
	void Bind() const;
    GLuint GetId() const;

private:
	GLuint m_Vao;
	uint32_t m_NumElements;
};
