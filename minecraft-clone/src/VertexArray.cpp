#include "VertexArray.h"

VertexArray::VertexArray() {
	glGenVertexArrays(1, &m_RendererID);
};

VertexArray::~VertexArray() {
	glDeleteVertexArrays(1, &m_RendererID);
};

void VertexArray::Bind() const {
	glBindVertexArray(m_RendererID);
};

void VertexArray::Unbind() const {
	glBindVertexArray(0);
};

void VertexArray::AddBuffer(const VertexBuffer& vbo, const VertexBufferLayout& layout)
{
	Bind();
	vbo.Bind();
	const auto& elements = layout.GetElements();
	unsigned int offset = 0;
	for (unsigned int i = 0; i < elements.size(); i++) {
		const auto& element = elements[i];
		// tell opengl what is the layout of our data
		glEnableVertexAttribArray(i); // enable the vertex attribute (takes the index to be enabled)
		// id, component count (2 because it's 2d), type of data, normalized, stride (amount of bytes between each vertex), position (offset)
		glVertexAttribPointer(i, element.count, element.type, element.normalized, layout.GetStride(), (const void*)offset); // this line of code links the vbo to the vao
		offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
	}
}