#pragma once
#include <GL/glew.h>
#include <vector>

struct VertexBufferElement
{
	VertexBufferElement(unsigned int type, unsigned int count, unsigned char normalized) :
		type(type), count(count), normalized(normalized) {};

	static unsigned int GetSizeOfType(unsigned int type) {
		switch (type)
		{
		case GL_DOUBLE: 
			return 8;
		case GL_FLOAT:			
		case GL_UNSIGNED_INT:
		case GL_INT:
			return 4;
		case GL_SHORT:
		case GL_UNSIGNED_SHORT:
			return 2;
		case GL_BYTE:
		case GL_UNSIGNED_BYTE:
			return 1;
		}

		return 0;
	}

	unsigned int type;
	unsigned int count;
	unsigned char normalized;
};

class VertexBufferLayout {
public:
	VertexBufferLayout() : m_Stride(0) {};

	template <typename T>
	void Push(unsigned int count) {
		//static_assert(false);
	}

	inline const std::vector<VertexBufferElement> GetElements() const { return m_Elements; };
	inline unsigned int GetStride() const { return m_Stride; }
private:
	std::vector<VertexBufferElement> m_Elements;
	unsigned int m_Stride;
};

template <>
inline void VertexBufferLayout::Push<float>(unsigned int count) {
	m_Elements.push_back(VertexBufferElement(GL_FLOAT, count, GL_FALSE));
	m_Stride += count * VertexBufferElement::GetSizeOfType(GL_FLOAT);
}

template <>
inline void VertexBufferLayout::Push<unsigned int>(unsigned int count) {
	m_Elements.push_back(VertexBufferElement(GL_UNSIGNED_INT, count, GL_FALSE));
	m_Stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_INT);
}

template <>
inline void VertexBufferLayout::Push<unsigned char>(unsigned int count) {
	m_Elements.push_back(VertexBufferElement(GL_UNSIGNED_BYTE, count, GL_TRUE));
	m_Stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_BYTE);
}