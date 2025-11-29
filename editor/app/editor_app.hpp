#pragma once

#include <cstdint>
#include <filesystem>
#include <string>

namespace wave::editor::app
{
    namespace fs = std::filesystem;

    struct EditorAppConfig
    {
        std::uint32_t window_width  = 1280;
        std::uint32_t window_height = 720;
        std::string   window_title  = "Wave Editor";
    };

    class EditorApp
    {
    public:
        // Run the editor application.
        //
        // executable_path should be the absolute path to the current binary.
        // Returns 0 on clean shutdown, non-zero on failure.
        static int run(const EditorAppConfig& config,
                       const fs::path& executable_path);
    };

} // namespace wave::editor::app
