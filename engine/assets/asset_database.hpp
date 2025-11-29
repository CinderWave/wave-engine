#pragma once

#include "asset_metadata.hpp"

#include <unordered_map>
#include <vector>
#include <filesystem>
#include <mutex>

namespace wave::engine::assets {

namespace fs = std::filesystem;

// Central registry of all assets under the asset root.
// Integrates with FileWatcher + JobSystem externally.
class AssetDatabase final {
public:
    AssetDatabase() = default;
    ~AssetDatabase() = default;

    AssetDatabase(const AssetDatabase&) = delete;
    AssetDatabase& operator=(const AssetDatabase&) = delete;

    AssetDatabase(AssetDatabase&&) noexcept = delete;
    AssetDatabase& operator=(AssetDatabase&&) noexcept = delete;

    // Initialize with a root asset directory.
    void initialize(const fs::path& assetRoot);

    const fs::path& root() const { return m_root; }

    // Populate database from disk on startup.
    void build_initial_scan();

    // Update metadata after file watcher events.
    void handle_file_created(const fs::path& path);
    void handle_file_modified(const fs::path& path);
    void handle_file_erased(const fs::path& path);

    // Access -------------------------------------------------------------------

    bool has(const fs::path& relativePath) const;

    const AssetMetadata* get_by_path(const fs::path& relativePath) const;
          AssetMetadata* get_by_path(const fs::path& relativePath);

    const AssetMetadata* get_by_id(const AssetID& id) const;
          AssetMetadata* get_by_id(const AssetID& id);

    std::vector<const AssetMetadata*> all() const;

private:
    void import_file(const fs::path& absPath);
    void remove_entry(const fs::path& absPath);

    AssetType detect_type_from_extension(const fs::path& p) const;

private:
    fs::path m_root;

    std::unordered_map<std::string, AssetMetadata> m_byPath; // key: relative path string
    std::unordered_map<std::uint64_t, AssetMetadata*> m_byIDhi; // hi part lookup
    std::unordered_map<std::uint64_t, AssetMetadata*> m_byIDlo; // lo part lookup

    mutable std::mutex m_mutex;
};

} // namespace wave::engine::assets
