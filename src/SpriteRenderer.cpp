#include "SpriteRenderer.h"
#include "glm/gtc/matrix_transform.hpp"
#include "Config.h"

#define ZNEAR 0.1f
#define ZFAR 1000.0f
#define FOV 45.0f
#define HEIGHT 1080.0f
#define WIDTH 1920.0f

const std::array<float, 16> crosshair_vertices = {
    0.0f, 1.0f, 0.9375f, 1.0f, // bottom left
    0.0f, 0.0f, 0.9375f, 0.9375f, // top left
    1.0f, 1.0f, 1.0f, 1.0f, // bottom right
    1.0f, 0.0f, 1.0f, 0.9375f, // top right
};

void SpriteRenderer::Clear() const { glClear(GL_COLOR_BUFFER_BIT); }

void SpriteRenderer::SetMV(const glm::mat4& mv) { m_MV = mv; }

void SpriteRenderer::SetSkyColor(const glm::vec3& skyColor) { m_SkyColor = skyColor; }

void SpriteRenderer::Init()
{
    m_Proj = glm::ortho(0.0f, WIDTH, 0.0f, HEIGHT, -1.0f, 1.0f);
    m_Shader = std::make_unique<Shader>(std::string(SOURCE_DIR) + "/res/shaders/shader_sprite.vert",
        std::string(SOURCE_DIR) + "/res/shaders/shader_sprite.frag");
    m_Shader->Bind();
    m_Shader->SetUniform1i("u_Texture", 0);

    m_Texture = std::make_unique<Texture>(std::string(SOURCE_DIR) + "/res/textures/gui.png");

    m_VBO = std::make_unique<VertexBuffer>();
    m_VBO->CreateStatic(crosshair_vertices.size() * sizeof(float), crosshair_vertices.data());
    m_VertexLayout.Push<float>(4); // vec2 pos + vec2 texture
    m_VAO = std::make_unique<VertexArray>();
    m_VAO->AddBuffer(*m_VBO, m_VertexLayout);
}

void SpriteRenderer::Draw() const
{
    glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO);
    float scale = 50.0f;
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3((WIDTH - scale) / 2, (HEIGHT - scale) / 2, 0.0f));
    model = glm::scale(model, glm::vec3(scale, scale, 1.0f));
    glm::mat4 mvp = m_Proj * model;
    // glm::vec4 texColor = { 1.0f, 1.0f, 1.0f, 1.0f };
    m_Shader->Bind();
    m_Shader->SetUniformMat4f("u_MVP", mvp);
    // m_Shader->SetUniform4fv("u_TexColor", texColor);
    m_Texture->Bind(0);
    m_VAO->Bind();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);
}