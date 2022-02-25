#pragma once
#include "VertexBuffer.h"
#include "VertexBufferLayout.hpp"

class VertexArray {
public:
	inline VertexArray() {
		GLCall(glGenVertexArrays(1, &m_RendererID));
	};
	~VertexArray() {
		GLCall(glDeleteVertexArrays(1, &m_RendererID));
	};

	void AddBuffer(const VertexBuffer& vbo, const VertexBufferLayout& layout);
	
	inline void Bind() const {
		GLCall(glBindVertexArray(m_RendererID));
	};
	void Unbind() const {
		GLCall(glBindVertexArray(0));
	};
private:
	unsigned int m_RendererID;
};

