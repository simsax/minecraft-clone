#pragma once
#include "FastNoiseLite.h"

// REMEMBER TO CHECK SCALE THINGY

class Noise
{
public:
    Noise();
    float CombinedNoise(int xCoord, int yCoord, unsigned int octaves, float frequency = 0.01f,
                        float lacunarity = 2.0f, float amplitude = 0.5f);
    float OctaveNoise(int xCoord, int yCoord, unsigned int octaves, float frequency = 0.01f,
                      float lacunarity = 2.0f, float amplitude = 0.5f);

private:
    static const unsigned int seed;
    FastNoiseLite m_FastNoise;
};
