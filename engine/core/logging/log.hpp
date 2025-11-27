#pragma once

#include <string>
#include <string_view>
#include <sstream>
#include <mutex>
#include <cstdint>

namespace wave::engine::core::logging
{
    enum class LogLevel : std::uint8_t
    {
        Trace,
        Debug,
        Info,
        Warn,
        Error,
        Critical
    };

    class Logger
    {
    public:
        // Non instantiable, purely static utility
        Logger() = delete;
        ~Logger() = delete;
        Logger(const Logger&) = delete;
        Logger(Logger&&) = delete;
        Logger& operator=(const Logger&) = delete;
        Logger& operator=(Logger&&) = delete;

        // Call once at startup (launcher, editor, game runtime)
        static void init(std::string app_name,
                         LogLevel min_level = LogLevel::Info) noexcept;

        static void shutdown() noexcept;

        static void set_min_level(LogLevel level) noexcept;
        [[nodiscard]] static LogLevel min_level() noexcept;

        template <typename... Args>
        static void trace(Args&&... args)
        {
            log(LogLevel::Trace, std::forward<Args>(args)...);
        }

        template <typename... Args>
        static void debug(Args&&... args)
        {
            log(LogLevel::Debug, std::forward<Args>(args)...);
        }

        template <typename... Args>
        static void info(Args&&... args)
        {
            log(LogLevel::Info, std::forward<Args>(args)...);
        }

        template <typename... Args>
        static void warn(Args&&... args)
        {
            log(LogLevel::Warn, std::forward<Args>(args)...);
        }

        template <typename... Args>
        static void error(Args&&... args)
        {
            log(LogLevel::Error, std::forward<Args>(args)...);
        }

        template <typename... Args>
        static void critical(Args&&... args)
        {
            log(LogLevel::Critical, std::forward<Args>(args)...);
        }

    private:
        template <typename... Args>
        static void log(LogLevel level, Args&&... args)
        {
            if (!should_log(level))
                return;

            std::string message = build_message(std::forward<Args>(args)...);
            write_line(level, message);
        }

        template <typename... Args>
        static std::string build_message(Args&&... args)
        {
            std::ostringstream oss;
            (oss << ... << std::forward<Args>(args));
            return oss.str();
        }

        [[nodiscard]] static bool should_log(LogLevel level) noexcept;
        static void write_line(LogLevel level, std::string_view message);

    private:
        static std::mutex s_mutex;
    };

} // namespace wave::engine::core::logging

// Convenience macros so call sites stay clean.
// Usage:
//   WAVE_LOG_INFO("Launcher started, version ", version);
//   WAVE_LOG_ERROR("Failed to load config: ", path);

#define WAVE_LOG_TRACE(...)    ::wave::engine::core::logging::Logger::trace(__VA_ARGS__)
#define WAVE_LOG_DEBUG(...)    ::wave::engine::core::logging::Logger::debug(__VA_ARGS__)
#define WAVE_LOG_INFO(...)     ::wave::engine::core::logging::Logger::info(__VA_ARGS__)
#define WAVE_LOG_WARN(...)     ::wave::engine::core::logging::Logger::warn(__VA_ARGS__)
#define WAVE_LOG_ERROR(...)    ::wave::engine::core::logging::Logger::error(__VA_ARGS__)
#define WAVE_LOG_CRITICAL(...) ::wave::engine::core::logging::Logger::critical(__VA_ARGS__)
