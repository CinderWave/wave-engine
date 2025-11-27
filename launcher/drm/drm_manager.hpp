#pragma once

#include <string>

namespace wave::launch
{
    class DrmManager
    {
    public:
        // Check a license file on disk.
        static bool verify_license_file(const std::string& path);

    private:
        static bool validate_key(const std::string& key);
    };
}
