#include <iostream>
#include <string>
#include <filesystem>
#include <cstdlib>

#include "engine_core.hpp"
#include "drm_manager.hpp"
#include "engine/core/logging/log.hpp"

namespace wave::launcher
{
    namespace fs = std::filesystem;

    // ------------------------------------------------------------
    // Menu model
    // ------------------------------------------------------------

    enum class MenuAction
    {
        None,
        LaunchEditor,
        LaunchGame,
        ManageProfiles,
        Settings,
        Quit
    };

    // ------------------------------------------------------------
    // Utility / UI helpers
    // ------------------------------------------------------------

    void print_banner()
    {
        std::cout << "=========================================\n";
        std::cout << "           Wave Engine Launcher          \n";
        std::cout << "=========================================\n";
    }

    void print_version()
    {
        std::cout << "Engine version: " << wave::EngineCore::version_string() << "\n";
        std::cout << "\n";
    }

    void print_menu()
    {
        std::cout << "Main Menu:\n";
        std::cout << "  1) Launch Wave Editor\n";
        std::cout << "  2) Launch Game (coming later)\n";
        std::cout << "  3) Profiles & saves (stub)\n";
        std::cout << "  4) Settings (stub)\n";
        std::cout << "  5) Quit\n";
        std::cout << "Select an option: ";
    }

    MenuAction parse_choice(const std::string& line)
    {
        if (line == "1") return MenuAction::LaunchEditor;
        if (line == "2") return MenuAction::LaunchGame;
        if (line == "3") return MenuAction::ManageProfiles;
        if (line == "4") return MenuAction::Settings;
        if (line == "5") return MenuAction::Quit;
        return MenuAction::None;
    }

    // ------------------------------------------------------------
    // Process helpers
    // ------------------------------------------------------------

    fs::path get_editor_binary_path()
    {
        // For now we assume launcher and editor are in the same folder:
        //   build/bin/wave_launcher
        //   build/bin/wave_editor
        fs::path current = fs::current_path();

    #if defined(_WIN32)
        fs::path editor = current / "wave_editor.exe";
    #else
        fs::path editor = current / "wave_editor";
    #endif

        return editor;
    }

    // ------------------------------------------------------------
    // Action handlers
    // ------------------------------------------------------------

    void handle_launch_editor()
    {
        fs::path editor = get_editor_binary_path();

        if (!fs::exists(editor))
        {
            std::cout << "\n[launcher] Could not find editor binary at:\n"
                      << "  " << editor << "\n";
            std::cout << "Make sure wave_editor is built and located next to wave_launcher.\n\n";

            WAVE_LOG_ERROR("[launcher] Editor binary not found at: ", editor.string());
            return;
        }

        std::cout << "\n[launcher] Launching Wave Editor...\n";
        std::cout << "  Path: " << editor << "\n";

        WAVE_LOG_INFO("[launcher] Launching Wave Editor at path: ", editor.string());

        // Blocking launch for now.
        int result = std::system(editor.string().c_str());

        if (result == -1)
        {
            std::cout << "[launcher] Failed to start editor process.\n\n";
            WAVE_LOG_ERROR("[launcher] Failed to start editor process.");
        }
        else
        {
            std::cout << "[launcher] Editor exited with code " << result << ".\n\n";
            WAVE_LOG_INFO("[launcher] Editor exited with code ", result, ".");
        }
    }

    void handle_launch_game()
    {
        std::cout << "\n[launcher] Game launch is not wired yet.\n";
        std::cout << "  This will start Echogenesis once the core loop exists.\n\n";

        WAVE_LOG_INFO("[launcher] Game launch requested but not implemented yet.");
    }

    void handle_profiles()
    {
        std::cout << "\n[launcher] Profiles & saves screen is a stub for now.\n";
        std::cout << "  Plan: player profiles, save slots, cloud sync hooks.\n\n";

        WAVE_LOG_DEBUG("[launcher] Profiles & saves stub accessed.");
    }

    void handle_settings()
    {
        std::cout << "\n[launcher] Settings screen is a stub for now.\n";
        std::cout << "  Plan: graphics presets, engine flags, debug options, etc.\n\n";

        WAVE_LOG_DEBUG("[launcher] Settings stub accessed.");
    }
} // namespace wave::launcher

// ------------------------------------------------------------
// Program entry
// ------------------------------------------------------------

int main()
{
    using wave::engine::core::logging::Logger;
    using wave::engine::core::logging::LogLevel;
    using namespace wave::launcher;

    try
    {
        Logger::init("WaveLauncher", LogLevel::Info);
        WAVE_LOG_INFO("Wave Launcher starting.");

        // DRM gate: use the dev-friendly manager in wave::launch
        wave::launch::DrmManager drm;
        if (!drm.verify_license_file("license.txt"))
        {
            std::cout << "Access denied: valid license required.\n";
            WAVE_LOG_WARN("Access denied: valid license required.");
            Logger::shutdown();
            return 1;
        }

        print_banner();
        print_version();

        bool running = true;
        while (running)
        {
            print_menu();

            std::string input;
            if (!std::getline(std::cin, input))
            {
                std::cout << "\nInput stream closed, exiting launcher.\n";
                WAVE_LOG_INFO("Input stream closed, exiting launcher.");
                break;
            }

            MenuAction action = parse_choice(input);

            switch (action)
            {
                case MenuAction::LaunchEditor:
                    handle_launch_editor();
                    break;

                case MenuAction::LaunchGame:
                    handle_launch_game();
                    break;

                case MenuAction::ManageProfiles:
                    handle_profiles();
                    break;

                case MenuAction::Settings:
                    handle_settings();
                    break;

                case MenuAction::Quit:
                    std::cout << "\nGoodbye.\n";
                    WAVE_LOG_INFO("Launcher received Quit command. Shutting down.");
                    running = false;
                    break;

                case MenuAction::None:
                default:
                    std::cout << "\nUnrecognised option. Please choose 1–5.\n\n";
                    WAVE_LOG_WARN("User entered unrecognised menu option: ", input);
                    break;
            }
        }

        Logger::shutdown();
        return 0;
    }
    catch (const std::exception& e)
    {
        WAVE_LOG_CRITICAL("Launcher fatal error: ", e.what());
        Logger::shutdown();
        return 1;
    }
}
