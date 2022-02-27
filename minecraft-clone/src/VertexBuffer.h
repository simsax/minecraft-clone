#pragma once
#include <GL/glew.h>

class VertexBuffer {
public:
	VertexBuffer();
	~VertexBuffer();

	void CreateStatic(unsigned int size, const void* data) const;
	void CreateDynamic(unsigned int size) const;
	void Bind() const;
	void UnBind() const;
	void SendData(unsigned int size, const void* data);
private:
	unsigned int m_RendererID;
};