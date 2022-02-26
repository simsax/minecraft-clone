#pragma once
#include <GL/glew.h>

class VertexBuffer {
public:
	VertexBuffer(unsigned int size, const void* data);
	~VertexBuffer();

	void Bind() const;
	void UnBind() const;
	void SendData(unsigned int size, const void* data);
private:
	unsigned int m_RendererID;
};