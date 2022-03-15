#include "Noise.h"
#include <glm/gtc/noise.hpp>
#include <iostream>

namespace noise {
	float Perlin2D(int xCoord, int yCoord, float scale, float frequency, float amplitude, int octaves, const std::vector<std::vector<int>>& offsets)
	{
		float xf = xCoord / scale;
		float yf = yCoord / scale;

		float result = 0.0f;

		for (int oct = 0; oct < octaves; oct++) {
			if (!offsets.empty())
				result += glm::perlin(glm::vec2(xf * frequency + offsets[oct][0], yf * frequency + offsets[oct][1])) * amplitude;
			else {
				result += glm::perlin(glm::vec2(xf * frequency, yf * frequency)) * amplitude;
			}
			frequency *= 2;
			amplitude /= 2;
		}

		return result;
	}

	float Perlin3D(int xCoord, int yCoord, int zCoord, float scale, float frequency, float amplitude, int octaves, const std::vector<std::vector<int>>& offsets)
	{
		float xf = xCoord / scale;
		float yf = yCoord / scale;
		float zf = zCoord / scale;

		float result = 0.0f;

		for (int oct = 0; oct < octaves; oct++) {
			if (!offsets.empty())
				result += glm::perlin(glm::vec3(xf * frequency + offsets[oct][0], yf * frequency + offsets[oct][1], zf * frequency + offsets[oct][2])) * amplitude;
			else {
				result += glm::perlin(glm::vec3(xf * frequency, yf * frequency, zf * frequency)) * amplitude;
			}
			frequency *= 2;
			amplitude /= 2;
		}

		return result;
	}
}