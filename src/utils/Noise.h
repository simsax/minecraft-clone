#pragma once

#include "FastNoise/FastNoise.h"

class Noise {
public:
    Noise();

    float OctaveNoiseSingle(float xCoord, float yCoord, int octaves, float frequency = 0.01f,
                            float lacunarity = 2.0f, float amplitude = 0.5f);

    template<size_t X, size_t Y>
    std::array<float, X * Y>
    OctaveNoiseGrid(int xStart, int yStart, int octaves, float frequency = 0.01f,
                    float lacunarity = 2.0f, float amplitude = 0.5f);

private:
    static const unsigned int seed;
    FastNoise::SmartNode<FastNoise::Simplex> m_Simplex;
    FastNoise::SmartNode<FastNoise::FractalFBm> m_Fractal;
};

template<size_t X, size_t Y>
std::array<float, X * Y>
Noise::OctaveNoiseGrid(int xStart, int yStart, int octaves, float frequency, float lacunarity,
                       float amplitude) {
    m_Fractal->SetOctaveCount(octaves);
    m_Fractal->SetLacunarity(lacunarity);
    m_Fractal->SetGain(amplitude);
    std::array<float, X * Y> noise = {};
    m_Fractal->GenUniformGrid2D(noise.data(), xStart, yStart, X, Y, frequency, seed);
    return noise;
}
