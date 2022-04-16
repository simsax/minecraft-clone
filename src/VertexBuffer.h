#pragma once
#include <GL/glew.h>

class VertexBuffer {
public:
	VertexBuffer();
	~VertexBuffer();

	void CreateStatic(GLsizeiptr size, const void* data) const;
	void CreateDynamic(GLsizeiptr size) const;
	void Bind() const;
	void UnBind() const;
	void SendData(GLsizeiptr size, const void* data);
private:
	unsigned int m_RendererID;
};