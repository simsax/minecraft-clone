#include "SkyEntities.h"
#include "../utils/Logger.h"

namespace SkyColors {
	static constexpr glm::vec4 tc1 = { 107.0f / 255.0f, 187.0f / 255.0f, 1.0f, 1.0f };
	static constexpr glm::vec4 tc2 = { 24.0f / 255.0f, 29.0f / 255.0f, 38.0f / 255.0f, 1.0f };
	static constexpr glm::vec4 bc1 = { 225.0f / 255.0f, 240.0f / 255.0f, 240 / 255.0f, 1.0f };
	static constexpr glm::vec4 bc2 = { 242.0f / 255.0f, 110.0f / 255.0f, 34.0f / 255.0f, 1.0f };
};

SkyEntities::SkyEntities(SkyRenderer* skyRenderer, int& screenHeight)
	: m_AmbientStrength(0.5f)
	, m_SkyRenderer(skyRenderer)
	, m_ScreenHeight(screenHeight)
	, m_Sun("sun", "sun1.png", glm::vec3(0), glm::vec3(400.0f, 1.0f, 400.0f))
	, m_Moon("moon", "moon.png", glm::vec3(0), glm::vec3(300.0f, 1.0f, 300.0f))
	, m_Stars("star", "star2.png", glm::vec3(0), glm::vec3(30.0f, 1.0f, 30.0f))
	, m_Clouds("clouds", "clouds.png", glm::vec3(0), glm::vec3(10000.0f, 1.0f, 10000.0f))
	, m_SunDir(0, 0, 0)
	, m_SkyColor(0, 0, 0, 1)
	, m_StarsAlpha(0.0f)
	, m_CloudsAlpha(0.0f)
{
}

void SkyEntities::Update(float deltaTime, const glm::vec3& position, const glm::vec3& frontVector)
{
	// slight offset on the x axis because it looks better
	glm::vec3 position2 = { position[0] - 50, position[1], position[2] };
	m_Sun.Update(deltaTime, position2);
	m_Moon.Update(deltaTime, position2);
	m_Stars.Update(deltaTime, position);
	m_Clouds.Update(deltaTime, position);
	m_SunDir = glm::normalize(m_Sun.GetPosition() - position);

	// update sky color
	static constexpr glm::vec3 upVector = { 0, 1, 0 };
	float d = glm::dot(upVector, m_SunDir);
	float x = glm::dot(upVector, frontVector);

	if (d >= 0) {
		glm::vec4 color = d * SkyColors::tc1 + (1 - d) * SkyColors::tc2;
		m_SkyColor = d * SkyColors::bc1 + (1 - d) * SkyColors::bc2;
		m_SkyRenderer->SetSkyColor(color);
		m_SkyRenderer->SetFogColor(m_SkyColor);
		m_StarsAlpha = 1 - d;
		if (d < 0.3)
			m_CloudsAlpha = d / 0.3f * 0.6f + 0.2f;
		else
			m_CloudsAlpha = 0.8f;
	}
	else {
		d = std::max(5 * d, -1.0f);
		m_SkyColor = -d * SkyColors::tc2 + (1 + d) * SkyColors::bc2;
		m_SkyRenderer->SetSkyColor(SkyColors::tc2);
		m_SkyRenderer->SetFogColor(m_SkyColor);
		m_StarsAlpha = 1;
		m_CloudsAlpha = 0.2f;
	}

	float lowerLimit = m_ScreenHeight * (-glm::asin(x) * 0.5f / glm::radians(35.0f) + 0.5f);
	m_SkyRenderer->SetLowerLimit(lowerLimit);


	// // update ambient strength
	// float dot = glm::dot(upVector, m_SunDir);

	// if (dot >= 0) {
	//     m_AmbientStrength = dot * 0.5f + (1 - dot) * 0.1f;
	// } else {
	//     m_AmbientStrength = -dot * 0.05f + (1 + dot) * 0.1f;
	// }
}

void SkyEntities::Render(QuadRenderer& renderer)
{
	m_Clouds.Render(renderer, m_CloudsAlpha);
	m_Sun.Render(renderer);
	m_Moon.Render(renderer);
	m_Stars.Render(renderer, m_StarsAlpha);
}

float SkyEntities::GetAmbientStrength() const { return m_AmbientStrength; }

glm::vec4 SkyEntities::GetSkyColor() const { return m_SkyColor; }

glm::vec3 SkyEntities::GetSunDir() const { return -m_SunDir; }

bool SkyEntities::IsDay() const
{
	if (m_Sun.GetPosition().y > 40)
		return true;
	else
		return false;
}
