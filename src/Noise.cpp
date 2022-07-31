#include "Noise.h"
#include <iostream>

/* const unsigned int Noise::seed = time(NULL); */
const unsigned int Noise::seed = 1;

Noise::Noise()
{
    m_FastNoise.SetSeed(seed);
    m_FastNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
}

float Noise::CombinedNoise(int xCoord, int yCoord, unsigned int octaves, float frequency,
    float lacunarity, float amplitude)
{
    int offset
        = static_cast<int>(OctaveNoise(xCoord, yCoord, octaves, frequency, lacunarity, amplitude));
    return OctaveNoise(xCoord + offset, yCoord, octaves, frequency, lacunarity, amplitude);
}

float Noise::OctaveNoise(int xCoord, int yCoord, unsigned int octaves, float frequency,
    float lacunarity, float amplitude)
{
    m_FastNoise.SetFrequency(frequency);
    m_FastNoise.SetFractalType(FastNoiseLite::FractalType_FBm);
    m_FastNoise.SetFractalOctaves(octaves);
    m_FastNoise.SetFractalLacunarity(lacunarity);
    m_FastNoise.SetFractalGain(amplitude);

    return m_FastNoise.GetNoise((float)xCoord, (float)yCoord);
}
