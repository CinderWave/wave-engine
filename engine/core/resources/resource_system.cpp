#include "engine/core/resources/resource_system.hpp"

#include "engine/core/runtime/runtime.hpp"
#include "engine/core/environment/environment.hpp"
#include "engine/core/filesystem/filesystem.hpp"
#include "engine/core/logging/log.hpp"

namespace wave::engine::core::resources
{
    using wave::engine::core::runtime::environment;
    using wave::engine::core::filesystem::read_text_file;
    using wave::engine::core::filesystem::read_binary_file;
    using wave::engine::core::logging::Logger;

    bool     ResourceSystem::s_initialized = false;
    fs::path ResourceSystem::s_resourceRoot;

    void ResourceSystem::initialize()
    {
        if (s_initialized)
            return;

        // Base this on the core Environment.
        const auto& env = environment();

        // Convention: resources live under <engine_root>/resources
        // If your Environment exposes a specific helper, you can swap this:
        //
        //   s_resourceRoot = env.resources_path();
        //
        s_resourceRoot = env.root_path() / "resources";

        if (!fs::exists(s_resourceRoot))
        {
            // Not fatal for now, but we log it. Some tools might still run
            // without resources (e.g. headless tests).
            WAVE_LOG_WARN("[resources] Resource root does not exist: ", s_resourceRoot.string());
        }
        else
        {
            WAVE_LOG_INFO("[resources] Resource root: ", s_resourceRoot.string());
        }

        s_initialized = true;
    }

    const fs::path& ResourceSystem::resource_root()
    {
        if (!s_initialized)
            initialize();

        return s_resourceRoot;
    }

    fs::path ResourceSystem::resolve(std::string_view relative)
    {
        if (!s_initialized)
            initialize();

        fs::path rel(relative);
        return s_resourceRoot / rel;
    }

    bool ResourceSystem::load_text(std::string_view relative, std::string& out)
    {
        auto path = resolve(relative);

        if (!fs::exists(path))
        {
            WAVE_LOG_ERROR("[resources] Text resource not found: ", path.string());
            return false;
        }

        if (!read_text_file(path, out))
        {
            WAVE_LOG_ERROR("[resources] Failed to read text resource: ", path.string());
            return false;
        }

        return true;
    }

    bool ResourceSystem::load_binary(std::string_view relative, std::vector<std::uint8_t>& out)
    {
        auto path = resolve(relative);

        if (!fs::exists(path))
        {
            WAVE_LOG_ERROR("[resources] Binary resource not found: ", path.string());
            return false;
        }

        if (!read_binary_file(path, out))
        {
            WAVE_LOG_ERROR("[resources] Failed to read binary resource: ", path.string());
            return false;
        }

        return true;
    }

} // namespace wave::engine::core::resources
