#pragma once

#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

namespace wave::engine::core::resources
{
    namespace fs = std::filesystem;

    // Central access point for engine resources (shaders, editor assets, etc.).
    //
    // The resource system does NOT own any caching or hot-reload logic yet.
    // It just resolves paths and provides basic load helpers.
    //
    // Layout convention (under engine root):
    //
    //   <engine_root>/
    //       resources/
    //           shaders/
    //           editor/
    //           textures/
    //           ...
    //
    class ResourceSystem
    {
    public:
        ResourceSystem() = delete;

        // Initialize internal root path based on the runtime environment.
        // Safe to call multiple times; subsequent calls are no-ops once initialized.
        static void initialize();

        // Has the resource system resolved its root yet.
        static bool is_initialized() noexcept { return s_initialized; }

        // Base directory for all engine resources.
        static const fs::path& resource_root();

        // Resolve a path inside the resource root, e.g.:
        //   "shaders/editor/basic.vert"
        static fs::path resolve(std::string_view relative);

        // Load text file relative to the resource root.
        // Returns true on success, false on failure.
        static bool load_text(std::string_view relative, std::string& out);

        // Load binary file relative to the resource root.
        // Returns true on success, false on failure.
        static bool load_binary(std::string_view relative, std::vector<std::uint8_t>& out);

    private:
        static bool     s_initialized;
        static fs::path s_resourceRoot;
    };

} // namespace wave::engine::core::resources
