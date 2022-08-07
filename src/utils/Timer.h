#include <chrono>

class Timer {
public:
    Timer();
    ~Timer();
    void Stop();

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> m_Start;
    bool m_Stopped;
};