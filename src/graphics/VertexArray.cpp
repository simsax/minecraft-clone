#include "VertexArray.h"

int8_t VertexArray::s_BufferCount = 0;
int8_t VertexArray::s_CurrentlyBound = -1;

VertexArray::VertexArray()
	: m_Vao(0), m_NumElements(0) {
	m_BindId = s_BufferCount;
	s_BufferCount++;
};

void VertexArray::Init() {
	glCreateVertexArrays(1, &m_Vao);
}

VertexArray::~VertexArray() { glDeleteVertexArrays(1, &m_Vao); };

void VertexArray::Bind() const {
	if (s_CurrentlyBound != m_BindId) {
		glBindVertexArray(m_Vao);
		s_CurrentlyBound = m_BindId;
	}
}

void VertexArray::AddLayout(const VertexBufferLayout& layout, GLuint bindingIndex) {
	const auto& elements = layout.GetElements();
	unsigned int offset = 0;
	for (unsigned int i = m_NumElements; i < m_NumElements + elements.size(); i++) {
		const auto& element = elements[i - m_NumElements];
		glEnableVertexArrayAttrib(m_Vao, i);
		switch (element.type) {
		case GL_DOUBLE:
			glVertexArrayAttribLFormat(
				m_Vao, i, element.count, element.type, offset);
			break;
		case GL_BYTE:
		case GL_UNSIGNED_BYTE:
		case GL_SHORT:
		case GL_UNSIGNED_SHORT:
		case GL_INT:
		case GL_UNSIGNED_INT:
			glVertexArrayAttribIFormat(
				m_Vao, i, element.count, element.type, offset);
			break;
		default:
			glVertexArrayAttribFormat(m_Vao, i, element.count, element.type,
				element.normalized,
				offset);
			break;
		}
		if (element.instanced)
			glVertexArrayBindingDivisor(m_Vao, bindingIndex, 1);
		glVertexArrayAttribBinding(m_Vao, i, bindingIndex);
		offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
	}
	m_NumElements += elements.size();
}

GLuint VertexArray::GetId() const {
	return m_Vao;
}
