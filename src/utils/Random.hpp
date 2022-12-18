#pragma once

#include <random>

class Random {
public:
    Random(const Random&) = delete;

    Random& operator=(const Random&) = delete;

    static Random& GetInstance()
    {
        static Random random;
        return random;
    }

    template <typename T> static T Uniform(T min, T max);
    static double Normal(double mean, double std);  

private:
    Random()
        : m_Gen(m_Rd()) {};

    std::random_device m_Rd;
    std::mt19937 m_Gen;
};

template <> inline double Random::Uniform<double>(double min, double max)
{
    std::uniform_real_distribution<> dis(min, max);
    auto& gen = Random::GetInstance().m_Gen;
    return dis(gen);
};

template <> inline int Random::Uniform<int>(int min, int max)
{
    std::uniform_int_distribution<> dis(min, max);
    auto& gen = Random::GetInstance().m_Gen;
    return dis(gen);
};


inline double Random::Normal(double mean, double std) {
    std::normal_distribution<> dis(mean, std);
    auto& gen = Random::GetInstance().m_Gen;
    return dis(gen);
}
