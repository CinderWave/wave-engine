#include "file_watcher.hpp"

#include <algorithm>

namespace wave::engine::core::filesystem {

// -----------------------------------------------------------------------------
// Construction / setup
// -----------------------------------------------------------------------------

FileWatcher::FileWatcher(fs::path root, bool recursive) {
    set_root(std::move(root), recursive);
}

void FileWatcher::set_root(fs::path root, bool recursive) {
    m_root      = std::move(root);
    m_recursive = recursive;

    clear_snapshot();
    m_events.clear();

    if (!m_root.empty() && fs::exists(m_root) && fs::is_directory(m_root)) {
        snapshot_current(m_snapshot);
    }
}

void FileWatcher::clear_snapshot() {
    m_snapshot.clear();
}

// -----------------------------------------------------------------------------
// Update loop
// -----------------------------------------------------------------------------

void FileWatcher::update() {
    if (m_root.empty() || !fs::exists(m_root) || !fs::is_directory(m_root)) {
        // Root missing; report nothing and clear snapshot so new root content
        // will be treated as "created" when it reappears.
        clear_snapshot();
        return;
    }

    std::unordered_map<std::string, EntryInfo> newSnapshot;
    snapshot_current(newSnapshot);
    compute_diff(newSnapshot);
    m_snapshot = std::move(newSnapshot);
}

std::vector<FileChangeEvent> FileWatcher::poll_events() {
    std::vector<FileChangeEvent> result;
    result.swap(m_events);
    return result;
}

// -----------------------------------------------------------------------------
// Snapshot
// -----------------------------------------------------------------------------

void FileWatcher::snapshot_current(std::unordered_map<std::string, EntryInfo>& outSnapshot) {
    const auto now = std::chrono::steady_clock::now(); // used only for events

    (void)now; // silence unused warning in this function; kept for symmetry

    auto addEntry = [&outSnapshot](const fs::directory_entry& entry) {
        if (!entry.is_regular_file() && !entry.is_directory()) {
            return;
        }

        const auto p = entry.path();
        std::string key = p.lexically_normal().generic_string();

        EntryInfo info{};
        std::error_code ec;

        info.lastWrite = fs::last_write_time(p, ec);
        if (ec) {
            info.lastWrite = TimePoint{};
        }

        info.size = entry.is_regular_file() ? entry.file_size(ec) : 0;
        if (ec) {
            info.size = 0;
        }

        outSnapshot.emplace(std::move(key), info);
    };

    std::error_code ec;
    if (m_recursive) {
        for (fs::recursive_directory_iterator it(m_root, ec), end; it != end && !ec; it.increment(ec)) {
            addEntry(*it);
        }
    } else {
        for (fs::directory_iterator it(m_root, ec), end; it != end && !ec; it.increment(ec)) {
            addEntry(*it);
        }
    }
}

// -----------------------------------------------------------------------------
// Diff computation
// -----------------------------------------------------------------------------

void FileWatcher::compute_diff(const std::unordered_map<std::string, EntryInfo>& newSnapshot) {
    const auto detectedAt = std::chrono::steady_clock::now();

    // Track matches by key.
    // Created: in newSnapshot but not in m_snapshot.
    // Erased: in m_snapshot but not in newSnapshot.
    // Modified: in both, but metadata differs.

    // 1) Created + Modified
    for (const auto& [pathKey, newInfo] : newSnapshot) {
        auto oldIt = m_snapshot.find(pathKey);

        if (oldIt == m_snapshot.end()) {
            // New file or directory
            FileChangeEvent evt{};
            evt.type       = FileChangeType::Created;
            evt.path       = fs::path(pathKey);
            evt.detectedAt = detectedAt;
            m_events.push_back(std::move(evt));
        } else {
            const EntryInfo& oldInfo = oldIt->second;

            const bool timeChanged = newInfo.lastWrite != oldInfo.lastWrite;
            const bool sizeChanged = newInfo.size != oldInfo.size;

            if (timeChanged || sizeChanged) {
                FileChangeEvent evt{};
                evt.type       = FileChangeType::Modified;
                evt.path       = fs::path(pathKey);
                evt.detectedAt = detectedAt;
                m_events.push_back(std::move(evt));
            }
        }
    }

    // 2) Erased
    for (const auto& [pathKey, oldInfo] : m_snapshot) {
        (void)oldInfo;

        if (newSnapshot.find(pathKey) == newSnapshot.end()) {
            FileChangeEvent evt{};
            evt.type       = FileChangeType::Erased;
            evt.path       = fs::path(pathKey);
            evt.detectedAt = detectedAt;
            m_events.push_back(std::move(evt));
        }
    }

    // 3) Renames (optional heuristic)
    //
    // Accurate rename detection is platform-specific; with pure snapshots
    // we cannot know for sure whether "A was renamed to B" or "A deleted, B created".
    //
    // For now we leave rename detection out or could add a heuristic later.
}

} // namespace wave::engine::core::filesystem
