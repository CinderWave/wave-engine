#pragma once

#include <cstdint>
#include <string>
#include <random>

namespace wave::engine::assets {

// Globally unique asset identifier.
// UUID v4 style, but very lightweight.
struct AssetID {
    std::uint64_t hi{0};
    std::uint64_t lo{0};

    bool valid() const {
        return hi != 0 || lo != 0;
    }

    bool operator==(const AssetID& other) const {
        return hi == other.hi && lo == other.lo;
    }

    bool operator!=(const AssetID& other) const {
        return !(*this == other);
    }

    static AssetID generate() {
        static thread_local std::mt19937_64 rng{ std::random_device{}() };
        AssetID id{};
        id.hi = rng();
        id.lo = rng();
        return id;
    }

    std::string to_string() const {
        char buf[33];
        std::snprintf(buf, sizeof(buf), "%016llx%016llx",
                      static_cast<unsigned long long>(hi),
                      static_cast<unsigned long long>(lo));
        return std::string(buf);
    }
};

} // namespace wave::engine::assets
