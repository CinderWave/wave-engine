#include "engine/core/logging/log.hpp"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <atomic>

namespace wave::engine::core::logging
{
    namespace
    {
        std::atomic<LogLevel> g_min_level{LogLevel::Info};
        std::string           g_app_name;
    } // namespace

    std::mutex Logger::s_mutex;

    static const char* level_to_string(LogLevel level) noexcept
    {
        switch (level)
        {
            case LogLevel::Trace:    return "TRACE";
            case LogLevel::Debug:    return "DEBUG";
            case LogLevel::Info:     return "INFO";
            case LogLevel::Warn:     return "WARN";
            case LogLevel::Error:    return "ERROR";
            case LogLevel::Critical: return "CRITICAL";
            default:                 return "UNKNOWN";
        }
    }

    void Logger::init(std::string app_name, LogLevel min_level) noexcept
    {
        g_app_name = std::move(app_name);
        g_min_level.store(min_level, std::memory_order_relaxed);
    }

    void Logger::shutdown() noexcept
    {
        // Placeholder for future sinks (files, remote endpoints).
        g_app_name.clear();
    }

    void Logger::set_min_level(LogLevel level) noexcept
    {
        g_min_level.store(level, std::memory_order_relaxed);
    }

    LogLevel Logger::min_level() noexcept
    {
        return g_min_level.load(std::memory_order_relaxed);
    }

    bool Logger::should_log(LogLevel level) noexcept
    {
        return static_cast<std::uint8_t>(level) >=
               static_cast<std::uint8_t>(g_min_level.load(std::memory_order_relaxed));
    }

    void Logger::write_line(LogLevel level, std::string_view message)
    {
        std::lock_guard<std::mutex> lock(s_mutex);

        // Timestamp in local time [HH:MM:SS]
        const auto now   = std::chrono::system_clock::now();
        const auto now_t = std::chrono::system_clock::to_time_t(now);
        std::tm     tm_buf{};

#if defined(_WIN32)
        localtime_s(&tm_buf, &now_t);
#else
        localtime_r(&now_t, &tm_buf);
#endif

        std::ostream& out =
            (level == LogLevel::Error || level == LogLevel::Critical)
                ? static_cast<std::ostream&>(std::cerr)
                : static_cast<std::ostream&>(std::clog);

        out << '['
            << std::setfill('0') << std::setw(2) << tm_buf.tm_hour << ':'
            << std::setfill('0') << std::setw(2) << tm_buf.tm_min << ':'
            << std::setfill('0') << std::setw(2) << tm_buf.tm_sec << ']';

        if (!g_app_name.empty())
        {
            out << '[' << g_app_name << ']';
        }

        out << '[' << level_to_string(level) << "] "
            << message << '\n';

        out.flush();
    }

} // namespace wave::engine::core::logging
