#include "asset_database.hpp"

#include <fstream>

namespace wave::engine::assets {

// -----------------------------------------------------------------------------
// Initialization
// -----------------------------------------------------------------------------

void AssetDatabase::initialize(const fs::path& assetRoot) {
    m_root = fs::weakly_canonical(assetRoot);
}

void AssetDatabase::build_initial_scan() {
    std::scoped_lock lock(m_mutex);

    m_byPath.clear();
    m_byIDhi.clear();
    m_byIDlo.clear();

    if (!fs::exists(m_root) || !fs::is_directory(m_root)) {
        return;
    }

    for (fs::recursive_directory_iterator it(m_root), end; it != end; it++) {
        const auto& entry = *it;

        if (!entry.is_regular_file()) {
            continue;
        }

        import_file(entry.path());
    }
}

// -----------------------------------------------------------------------------
// Helpers
// -----------------------------------------------------------------------------

AssetType AssetDatabase::detect_type_from_extension(const fs::path& p) const {
    const auto ext = p.extension().string();

    if (ext == ".png" || ext == ".jpg" || ext == ".jpeg") return AssetType::Texture;
    if (ext == ".obj" || ext == ".fbx" || ext == ".gltf") return AssetType::Mesh;
    if (ext == ".mat") return AssetType::Material;
    if (ext == ".vert" || ext == ".frag" || ext == ".comp") return AssetType::Shader;
    if (ext == ".scene") return AssetType::Scene;
    if (ext == ".cfg") return AssetType::Config;

    return AssetType::Unknown;
}

void AssetDatabase::import_file(const fs::path& absPath) {
    if (!fs::exists(absPath) || !fs::is_regular_file(absPath)) {
        return;
    }

    const fs::path rel = fs::relative(absPath, m_root);
    const std::string key = rel.generic_string();

    AssetMetadata meta;
    meta.id = AssetID::generate();
    meta.absolutePath = absPath;
    meta.relativePath = rel;

    std::error_code ec;
    meta.fileSize = fs::file_size(absPath, ec);
    meta.lastWriteTime = fs::last_write_time(absPath, ec);

    meta.type = detect_type_from_extension(absPath);

    auto it = m_byPath.find(key);
    if (it != m_byPath.end()) {
        it->second = meta;
    } else {
        m_byPath.emplace(key, meta);
    }

    auto& stored = m_byPath[key];
    m_byIDhi[stored.id.hi] = &stored;
    m_byIDlo[stored.id.lo] = &stored;
}

void AssetDatabase::remove_entry(const fs::path& absPath) {
    const fs::path rel = fs::relative(absPath, m_root);
    const std::string key = rel.generic_string();

    auto it = m_byPath.find(key);
    if (it == m_byPath.end()) {
        return;
    }

    const auto& meta = it->second;
    m_byIDhi.erase(meta.id.hi);
    m_byIDlo.erase(meta.id.lo);

    m_byPath.erase(it);
}

// -----------------------------------------------------------------------------
// FileWatcher events handling
// -----------------------------------------------------------------------------

void AssetDatabase::handle_file_created(const fs::path& path) {
    std::scoped_lock lock(m_mutex);
    import_file(path);
}

void AssetDatabase::handle_file_modified(const fs::path& path) {
    std::scoped_lock lock(m_mutex);

    const fs::path rel = fs::relative(path, m_root);
    const std::string key = rel.generic_string();

    auto it = m_byPath.find(key);
    if (it == m_byPath.end()) {
        import_file(path);
    } else {
        import_file(path);
        it->second.needsReimport = true;
    }
}

void AssetDatabase::handle_file_erased(const fs::path& path) {
    std::scoped_lock lock(m_mutex);
    remove_entry(path);
}

// -----------------------------------------------------------------------------
// Getters
// -----------------------------------------------------------------------------

bool AssetDatabase::has(const fs::path& relativePath) const {
    std::scoped_lock lock(m_mutex);
    return m_byPath.count(relativePath.generic_string()) > 0;
}

const AssetMetadata* AssetDatabase::get_by_path(const fs::path& relativePath) const {
    std::scoped_lock lock(m_mutex);
    auto it = m_byPath.find(relativePath.generic_string());
    return it != m_byPath.end() ? &it->second : nullptr;
}

AssetMetadata* AssetDatabase::get_by_path(const fs::path& relativePath) {
    std::scoped_lock lock(m_mutex);
    auto it = m_byPath.find(relativePath.generic_string());
    return it != m_byPath.end() ? &it->second : nullptr;
}

const AssetMetadata* AssetDatabase::get_by_id(const AssetID& id) const {
    std::scoped_lock lock(m_mutex);

    auto it = m_byIDhi.find(id.hi);
    if (it == m_byIDhi.end()) return nullptr;
    AssetMetadata* meta = it->second;
    return meta && meta->id.lo == id.lo ? meta : nullptr;
}

AssetMetadata* AssetDatabase::get_by_id(const AssetID& id) {
    std::scoped_lock lock(m_mutex);

    auto it = m_byIDhi.find(id.hi);
    if (it == m_byIDhi.end()) return nullptr;
    AssetMetadata* meta = it->second;
    return meta && meta->id.lo == id.lo ? meta : nullptr;
}

std::vector<const AssetMetadata*> AssetDatabase::all() const {
    std::scoped_lock lock(m_mutex);
    std::vector<const AssetMetadata*> out;
    out.reserve(m_byPath.size());

    for (const auto& [_, meta] : m_byPath) {
        out.push_back(&meta);
    }

    return out;
}

} // namespace wave::engine::assets
