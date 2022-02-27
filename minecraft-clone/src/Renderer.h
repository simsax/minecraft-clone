#pragma once
#include <GL/glew.h>
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"

class Renderer {
public:
	void Draw(const VertexArray& vao, const IndexBuffer& ibo, GLenum type, const Shader& shader) const;
	void DrawInstanced(const VertexArray& vao, const IndexBuffer& ibo, GLenum type, const Shader& shader, GLsizei num_instances) const;
	void Clear() const;
};