#pragma once

#include <string>
#include <filesystem>

#include "engine/core/logging/log.hpp"
#include "engine/core/environment/environment.hpp"

namespace wave::engine::core::runtime
{
    namespace fs = std::filesystem;

    using LogLevel    = wave::engine::core::logging::LogLevel;
    using Environment = wave::engine::core::environment::Environment;

    // Configuration used when bringing the engine runtime online.
    struct RuntimeConfig
    {
        std::string app_name       = "WaveApp";
        LogLevel    min_log_level  = LogLevel::Info;

        // Path to the currently running executable.
        // This is used by Environment to locate the engine root.
        fs::path    executable_path;
    };

    // Initialize core runtime systems (logging, time, environment).
    //
    // Safe to call more than once; subsequent calls are no-ops that return true
    // if the runtime is already initialized.
    //
    // Returns false if initialization fails (for example, if environment
    // initialization fails due to an invalid executable path).
    bool initialize(const RuntimeConfig& config);

    // Shut down core runtime systems.
    // Safe to call even if initialize() was never called.
    void shutdown() noexcept;

    // Has the runtime been initialized successfully?
    bool is_initialized() noexcept;

    // Access the global Environment instance.
    // Only valid after initialize() has returned true.
    const Environment& environment() noexcept;

} // namespace wave::engine::core::runtime
