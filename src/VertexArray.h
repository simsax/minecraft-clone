#pragma once
#include "VertexBuffer.h"
#include "VertexBufferLayout.hpp"

class VertexArray
{
public:
	VertexArray();
	~VertexArray();

	void AddBuffer(const VertexBuffer &vbo, const VertexBufferLayout &layout);
	void AddInstanceBuffer(const VertexBuffer &vbo, const VertexBufferLayout &layout, GLuint divisor);
	void Bind() const;
	void Unbind() const;

private:
	unsigned int m_RendererID;
	unsigned int m_NumElements;
};
