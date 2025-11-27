#pragma once

#include <string>

namespace wave
{
    class EngineCore
    {
    public:
        // For now just a tiny heartbeat.
        static std::string version_string();
    };
}
