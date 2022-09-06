#include "spdlog/spdlog.h"

class Logger {
public:
    static void Init();

    inline static std::shared_ptr<spdlog::logger> GetCpuLogger()  { return s_CpuLog; }
    inline static std::shared_ptr<spdlog::logger> GetGLLogger() { return s_GLLog; }
private:
    static std::shared_ptr<spdlog::logger> s_CpuLog;
    static std::shared_ptr<spdlog::logger> s_GLLog;
};

#ifdef NDEBUG

#define LOG_TRACE
#define LOG_INFO
#define LOG_WARN
#define LOG_ERROR
#define LOG_CRITICAL

#define GL_LOG_TRACE
#define GL_LOG_INFO
#define GL_LOG_WARN
#define GL_LOG_ERROR
#define GL_LOG_CRITICAL

#else

#define LOG_TRACE(...) Logger::GetCpuLogger()->trace(__VA_ARGS__)
#define LOG_INFO(...) Logger::GetCpuLogger()->info(__VA_ARGS__)
#define LOG_WARN(...) Logger::GetCpuLogger()->warn(__VA_ARGS__)
#define LOG_ERROR(...) Logger::GetCpuLogger()->error(__VA_ARGS__)
#define LOG_CRITICAL(...) Logger::GetCpuLogger()->critical(__VA_ARGS__)

#define GL_LOG_TRACE(...) Logger::GetGLLogger()->trace(__VA_ARGS__)
#define GL_LOG_INFO(...) Logger::GetGLLogger()->info(__VA_ARGS__)
#define GL_LOG_WARN(...) Logger::GetGLLogger()->warn(__VA_ARGS__)
#define GL_LOG_ERROR(...) Logger::GetGLLogger()->error(__VA_ARGS__)
#define GL_LOG_CRITICAL(...) Logger::GetGLLogger()->critical(__VA_ARGS__)

#endif
