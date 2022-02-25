#pragma once
#include "GLErrorManager.h"
#include <string>

class Texture
{
public:
	Texture(const std::string& path);
	~Texture();

	void Bind(unsigned int slot = 0) const; // each GPU has a number of slots to bind different textures
	void Unbind() const;
	inline int GetWidth() const { return m_Width; }
	inline int GetHeight() const { return m_Height; }
private:
	unsigned int m_RendererID;
	std::string m_FilePath;
	unsigned char* m_LocalBuffer; // contains the texture data
	int m_Width, m_Height, m_BPP; //BPP: bytes per pixel
};

