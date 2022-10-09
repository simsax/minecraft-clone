#include <chrono>
#include <string>

class Timer {
public:
    Timer(std::string name);
    ~Timer();
    void Stop();

private:
    std::string m_Name;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_Start;
    bool m_Stopped;
};
