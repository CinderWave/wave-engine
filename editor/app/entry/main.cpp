#include <iostream>
#include <string>

#include "engine_core.hpp" // If you want the version string, optional

namespace wave::launcher
{
    enum class MenuAction
    {
        None,
        LaunchEditor,
        LaunchGame,
        ManageProfiles,
        Settings,
        Quit
    };

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

    void handle_launch_editor()
    {
        // For now we just describe what would happen.
        // Later: actually spawn the editor process from here.
        std::cout << "\n[Launcher] Launching Wave Editor...\n";
        std::cout << "  (In a later step, this will spawn the actual editor executable.)\n\n";
    }

    void handle_launch_game()
    {
        std::cout << "\n[Launcher] Game launch is not wired yet.\n";
        std::cout << "  This will start Echogenesis once the core loop exists.\n\n";
    }

    void handle_profiles()
    {
        std::cout << "\n[Launcher] Profiles & saves screen is a stub for now.\n";
        std::cout << "  Plan: player profiles, save slots, cloud sync hooks.\n\n";
    }

    void handle_settings()
    {
        std::cout << "\n[Launcher] Settings screen is a stub for now.\n";
        std::cout << "  Plan: graphics presets, engine flags, debug options, etc.\n\n";
    }
}

int main()
{
    using namespace wave::launcher;

    try
    {
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
                    running = false;
                    break;

                case MenuAction::None:
                default:
                    std::cout << "\nUnrecognised option. Please choose 1–5.\n\n";
                    break;
            }
        }

        return 0;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Launcher fatal error: " << e.what() << "\n";
        return 1;
    }
}
