#include "drm_manager.hpp"

#include <fstream>
#include <string>

#include "engine/core/logging/log.hpp"
#include "engine/core/build/build_config.hpp"
#include "engine/core/utils/string_utils.hpp"

namespace wave::launch
{
    bool DrmManager::verify_license_file(const std::string& path)
    {
        using wave::engine::core::build::kDevFriendlyDrm;

        std::ifstream file(path);
        if (!file)
        {
            if (kDevFriendlyDrm)
            {
                WAVE_LOG_WARN("[launcher] No license file found at: ", path);
                WAVE_LOG_INFO("[launcher] Dev build: skipping DRM and continuing.");
                // Internal/dev builds: allow running without a license file.
                return true;
            }
            else
            {
                WAVE_LOG_ERROR("[launcher] No license file found at: ", path);
                WAVE_LOG_ERROR("[launcher] DRM check failed (public build requires a license).");
                return false;
            }
        }

        std::string key;
        std::getline(file, key);
        key = wave::engine::core::utils::trim(key);

        if (key.empty())
        {
            WAVE_LOG_ERROR("[launcher] License file is empty.");
            return false;
        }

        if (!validate_key(key))
        {
            WAVE_LOG_ERROR("[launcher] License key invalid.");
            return false;
        }

        WAVE_LOG_INFO("[launcher] License key accepted.");
        return true;
    }

    bool DrmManager::validate_key(const std::string& key)
    {
        // Temporary placeholder: this will be replaced by a real system later.
        // For now it just proves the pipeline for DRM exists.
        static const std::string devKey = "WAVE-DEV-LICENSE";

        return key == devKey;
    }
}
