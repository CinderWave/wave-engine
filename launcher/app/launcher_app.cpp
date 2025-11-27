#include "launcher_app.hpp"
#include "drm_manager.hpp"

#include <iostream>

namespace wave::launch
{
    int LauncherApp::run(const std::string& licensePath)
    {
        std::cout << "Wave Launcher starting...\n";

        if (!DrmManager::verify_license_file(licensePath))
        {
            std::cout << "Access denied: valid license required.\n";
            return 1;
        }

        // Later: choose between game/editor, handle arguments, profiles, etc.
        std::cout << "Launching Wave Engine environment...\n";

        return 0;
    }
}
