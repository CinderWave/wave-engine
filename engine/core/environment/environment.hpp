#pragma once

#include <string>
#include <filesystem>

namespace wave::engine::core::environment
{
    namespace fs = std::filesystem;

    class Environment
    {
    public:
        Environment() = default;

        // Initialize using the path to the executable.
        // The launcher/editor passes argv[0] or platform specific APIs.
        bool initialize(const fs::path& executable_path);

        // Core paths ----------------------------------------------------------

        [[nodiscard]] const fs::path& engine_root() const noexcept;
        [[nodiscard]] const fs::path& working_dir() const noexcept;

        // Standardized subpaths ----------------------------------------------

        [[nodiscard]] fs::path config_path() const;      // engine_root/config/
        [[nodiscard]] fs::path logs_path() const;        // engine_root/logs/
        [[nodiscard]] fs::path resources_path() const;   // engine_root/resources/
        [[nodiscard]] fs::path editor_path() const;      // engine_root/editor/
        [[nodiscard]] fs::path tools_path() const;       // engine_root/tools/

    private:
        fs::path m_engine_root;
        fs::path m_working_dir;
        bool     m_initialized = false;
    };

} // namespace wave::engine::core::environment
