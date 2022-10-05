#include "Renderer.h"
#include "glm/gtc/matrix_transform.hpp"
#include "../camera/Constants.h"
#include "../utils/Logger.h"

void Renderer::Init(int width, int height)
{
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    m_PersProj = glm::perspective(
        glm::radians(FOV), static_cast<float>(width) / static_cast<float>(height), ZNEAR, ZFAR);
    m_OrthoProj = glm::ortho(
        0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height), -1.0f, 1.0f);

    chunkRenderer.Init(&m_PersProj, &m_View);
    quadRenderer.Init(&m_PersProj, &m_View);
    guiRenderer.Init(&m_OrthoProj);
    skyRenderer.Init(&m_OrthoProj);
    skyRenderer.Resize(width, height);
}

void Renderer::Resize(int width, int height)
{
    m_PersProj = glm::perspective(
        glm::radians(FOV), static_cast<float>(width) / static_cast<float>(height), ZNEAR, ZFAR);
    m_OrthoProj = glm::ortho(
        0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height), -1.0f, 1.0f);
    skyRenderer.Resize(width, height);
}

void Renderer::Clear(const glm::vec4& skyColor)
{
    glClearColor(skyColor.x, skyColor.y, skyColor.z, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::SetViewMatrix(const glm::mat4& view) { m_View = view; }
