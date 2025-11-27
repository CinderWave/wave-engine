#include "engine/core/environment/environment.hpp"

#include <iostream>

namespace wave::engine::core::environment
{
    bool Environment::initialize(const fs::path& executable_path)
    {
        try
        {
            m_working_dir = fs::current_path();

            // Resolve engine root as the directory containing the executable.
            fs::path exe_abs = fs::absolute(executable_path);
            m_engine_root    = exe_abs.parent_path();

            m_initialized = true;
            return true;
        }
        catch (const std::exception& e)
        {
            std::cerr << "[environment] Initialization failed: " << e.what() << "\n";
            return false;
        }
    }

    const fs::path& Environment::engine_root() const noexcept
    {
        return m_engine_root;
    }

    const fs::path& Environment::working_dir() const noexcept
    {
        return m_working_dir;
    }

    fs::path Environment::config_path() const
    {
        return m_engine_root / "config";
    }

    fs::path Environment::logs_path() const
    {
        return m_engine_root / "logs";
    }

    fs::path Environment::resources_path() const
    {
        return m_engine_root / "resources";
    }

    fs::path Environment::editor_path() const
    {
        return m_engine_root / "editor";
    }

    fs::path Environment::tools_path() const
    {
        return m_engine_root / "tools";
    }

} // namespace wave::engine::core::environment
