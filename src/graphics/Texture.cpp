#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int8_t Texture::m_TextureCount = 0;
int8_t Texture::m_CurrentlyBound = -1;

Texture::Texture(std::string path)
    : m_TextureId(0)
    , m_LocalBuffer(nullptr)
    , m_Width(0)
    , m_Height(0)
    , m_BPP(0)
    , m_FilePath(std::move(path))
{
    m_BindId = m_TextureCount;
    m_TextureCount++;
}

void Texture::Init(uint32_t magFilter)
{
    // flip texture upside down because for opengl bottom left is the starting position
    stbi_set_flip_vertically_on_load(1);
    m_LocalBuffer = stbi_load(m_FilePath.c_str(), &m_Width, &m_Height, &m_BPP, 4);
    glCreateTextures(GL_TEXTURE_2D, 1, &m_TextureId);

    glTextureParameteri(m_TextureId, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTextureParameteri(m_TextureId, GL_TEXTURE_MAG_FILTER, magFilter);
    glTextureParameteri(m_TextureId, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(m_TextureId, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureParameteri(m_TextureId, GL_TEXTURE_MAX_LEVEL, 4);

    glTextureStorage2D(m_TextureId, 4, GL_RGBA8, m_Width, m_Height);
    // glTextureStorage2D(m_TextureId, 4, GL_SRGB8_ALPHA8, m_Width, m_Height);
    glTextureSubImage2D(
        m_TextureId, 0, 0, 0, m_Width, m_Height, GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer);
    glGenerateTextureMipmap(m_TextureId);
    if (m_LocalBuffer)
        stbi_image_free(m_LocalBuffer);
}

Texture::~Texture() { glDeleteTextures(1, &m_TextureId); }

void Texture::Bind(GLuint slot) const
{
    if (m_CurrentlyBound != m_BindId) {
        glBindTextureUnit(slot, m_TextureId);
        m_CurrentlyBound = m_BindId;
    }
}

Texture::Texture(Texture&& other) noexcept
{
    this->m_TextureId = other.m_TextureId;
    this->m_LocalBuffer = other.m_LocalBuffer;
    this->m_Width = other.m_Width;
    this->m_Height = other.m_Height;
    this->m_BPP = other.m_BPP;
    this->m_BindId = other.m_BindId;
    this->m_FilePath = other.m_FilePath;
    other.m_TextureId = 0;
    other.m_BindId = -1;
}

Texture& Texture::operator=(Texture&& other) noexcept
{
    if (this != &other) {
        this->m_TextureId = other.m_TextureId;
        this->m_LocalBuffer = other.m_LocalBuffer;
        this->m_Width = other.m_Width;
        this->m_Height = other.m_Height;
        this->m_BPP = other.m_BPP;
        this->m_BindId = other.m_BindId;
        this->m_FilePath = other.m_FilePath;
        other.m_TextureId = 0;
        other.m_BindId = -1;
    }
    return *this;
}
