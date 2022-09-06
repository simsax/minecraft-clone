#include "Logger.h"
#include "spdlog/sinks/stdout_color_sinks.h"

std::shared_ptr<spdlog::logger> Logger::s_CpuLog;
std::shared_ptr<spdlog::logger> Logger::s_GLLog;

void Logger::Init() {
    spdlog::set_pattern("[%T.%e] [%n] [%^%l%$] %v");
    s_CpuLog = spdlog::stdout_color_mt("CPU");
    s_CpuLog->set_level(spdlog::level::trace);
    s_GLLog = spdlog::stdout_color_mt("OpenGL");
    s_GLLog->set_level(spdlog::level::trace);
}
