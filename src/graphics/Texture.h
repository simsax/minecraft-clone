#pragma once
#include "GL/glew.h"
#include <string>

class Texture
{
public:
	Texture(std::string path);
	~Texture();

	void Bind(GLuint slot = 0) const;
	inline int GetWidth() const { return m_Width; }
	inline int GetHeight() const { return m_Height; }

private:
	uint32_t m_TextureId;
	std::string m_FilePath;
	GLvoid* m_LocalBuffer;
	int m_Width, m_Height, m_BPP;
    static int8_t m_TextureCount;
    static int8_t m_CurrentlyBound;
    int8_t m_BindId;
};
