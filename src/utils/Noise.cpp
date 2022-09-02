#include "Noise.h"
#include <iostream>

/* const unsigned int Noise::seed = time(NULL); */
const unsigned int Noise::seed = 1;

Noise::Noise() :
        m_Simplex(FastNoise::New<FastNoise::Simplex>()),
        m_Fractal(FastNoise::New<FastNoise::FractalFBm>())
{
    m_Fractal->SetSource(m_Simplex);
}

float
Noise::OctaveNoiseSingle(float xCoord, float yCoord, int octaves, float frequency, float lacunarity,
                         float amplitude) {
    m_Fractal->SetOctaveCount(octaves);
    m_Fractal->SetLacunarity(lacunarity);
    m_Fractal->SetGain(amplitude);
    return m_Fractal->GenSingle2D(xCoord * frequency, yCoord * frequency, seed);
}
