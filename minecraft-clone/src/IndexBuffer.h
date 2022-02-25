#pragma once

#include "GL/glew.h"

class IndexBuffer {
public:
	IndexBuffer(const void* data, GLsizeiptr size);
	~IndexBuffer();

	void Bind() const;
	void UnBind() const;
	unsigned int GetCount() const;
	void SetCount(unsigned int count);
private:
	unsigned int m_RendererID;
	unsigned int m_Count;
};

