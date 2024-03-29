#include "Timer.h"
#include <iostream>
#include <utility>

Timer::Timer(std::string name)
    : m_Name(std::move(name)), m_Stopped(false)
{
    m_Start = std::chrono::high_resolution_clock::now();
}

Timer::~Timer()
{
    if (!m_Stopped)
        Stop();
};

void Timer::Stop()
{
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - m_Start).count();
    double duration = elapsed * 0.001;
    m_Stopped = true;
    std::cout << m_Name << " - duration: " << duration << " ms\n";
};
