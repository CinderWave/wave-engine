#include "engine/core/runtime/runtime.hpp"

#include "engine/core/time/time.hpp"
#include "engine/core/build/build_config.hpp"

namespace wave::engine::core::runtime
{
    namespace
    {
        bool         g_initialized = false;
        Environment  g_environment;
        RuntimeConfig g_config;
    } // namespace

    bool initialize(const RuntimeConfig& config)
    {
        if (g_initialized)
            return true;

        // Store config for possible later inspection.
        g_config = config;

        // Initialize environment first: other systems may rely on paths.
        if (!g_environment.initialize(config.executable_path))
        {
            // Environment already prints/logs its own error messages.
            return false;
        }

        // Initialize logging if enabled by build config.
        if (wave::engine::core::build::kLoggingEnabled)
        {
            wave::engine::core::logging::Logger::init(
                config.app_name,
                config.min_log_level
            );
        }

        // Initialize high resolution timing.
        wave::engine::core::time::Time::initialize();

        g_initialized = true;
        return true;
    }

    void shutdown() noexcept
    {
        if (!g_initialized)
            return;

        // Shut down logging last, after any final messages.
        if (wave::engine::core::build::kLoggingEnabled)
        {
            wave::engine::core::logging::Logger::shutdown();
        }

        g_initialized = false;
    }

    bool is_initialized() noexcept
    {
        return g_initialized;
    }

    const Environment& environment() noexcept
    {
        return g_environment;
    }

} // namespace wave::engine::core::runtime
