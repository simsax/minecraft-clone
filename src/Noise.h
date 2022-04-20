#pragma once

#include "vector"

namespace noise {
	float CombinedNoise(int xCoord, int yCoord, float scale, float frequency, float amplitude, unsigned int octaves = 1);
    float OctaveNoise(int xCoord, int yCoord, float scale, float frequency, float amplitude, unsigned int octaves = 1);
    float Perlin2D(int xCoord, int yCoord, float scale, float frequency, float amplitude, int octaves = 1, const std::vector<std::vector<int>>& offsets = {});
	float Perlin3D(int xCoord, int yCoord, int zCoord, float scale, float frequency, float amplitude, int octaves = 1, const std::vector<std::vector<int>>& offsets = {});
}
