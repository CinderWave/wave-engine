#include <filesystem>

#include "editor/app/editor_app.hpp"

int main(int argc, char** argv)
{
    namespace fs = std::filesystem;
    using wave::editor::app::EditorApp;
    using wave::editor::app::EditorAppConfig;

    // Resolve executable path for runtime/environment init.
    fs::path executable_path = fs::absolute(argv[0]);

    // Basic initial window config for the internal editor.
    EditorAppConfig config;
    config.window_width  = 1600;
    config.window_height = 900;
    config.window_title  = "Wave Editor (Internal)";

    return EditorApp::run(config, executable_path);
}
