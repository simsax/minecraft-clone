#include "Noise.h"
#include <iostream>

/* const unsigned int Noise::seed = time(NULL); */
const unsigned int Noise::seed = 1;

Noise::Noise() :
        m_Perlin(FastNoise::New<FastNoise::Perlin>()),
        m_Fractal(FastNoise::New<FastNoise::FractalFBm>())
{
//    m_FastNoise.SetSeed(seed);
//    m_FastNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);

    m_Fractal->SetSource(m_Perlin);
}

//float Noise::CombinedNoise(int xCoord, int yCoord, unsigned int octaves, float frequency,
//    float lacunarity, float amplitude)
//{
//    int offset
//        = static_cast<int>(OctaveNoise(xCoord, yCoord, octaves, frequency, lacunarity, amplitude));
//    return OctaveNoise(xCoord + offset, yCoord, octaves, frequency, lacunarity, amplitude);
//}

float
Noise::OctaveNoiseSingle(float xCoord, float yCoord, int octaves, float frequency, float lacunarity,
                         float amplitude) {
    m_Fractal->SetOctaveCount(octaves);
    m_Fractal->SetLacunarity(lacunarity);
    m_Fractal->SetGain(amplitude);
    return m_Fractal->GenSingle2D(xCoord, yCoord, seed);
}
