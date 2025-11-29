#pragma once

#include "asset_id.hpp"

#include <string>
#include <filesystem>
#include <cstdint>

namespace wave::engine::assets {

namespace fs = std::filesystem;

// Basic asset types; extend later.
enum class AssetType {
    Unknown,
    Texture,
    Mesh,
    Material,
    Shader,
    Scene,
    Config,
    Folder
};

// Metadata stored for each asset entry in the database.
struct AssetMetadata {
    AssetID id;
    AssetType type{AssetType::Unknown};

    fs::path absolutePath;   // full actual path
    fs::path relativePath;   // relative to asset root

    std::uint64_t fileSize{0};
    fs::file_time_type lastWriteTime{};

    // Engine may use this to track import pipeline changes later.
    bool needsReimport{false};

    bool valid() const {
        return id.valid();
    }
};

} // namespace wave::engine::assets
