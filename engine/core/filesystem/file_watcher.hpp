#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <filesystem>
#include <cstdint>

namespace wave::engine::core::filesystem {

namespace fs = std::filesystem;

enum class FileChangeType {
    Created,
    Modified,
    Erased,
    Renamed
};

struct FileChangeEvent {
    FileChangeType type;

    // New/current path (for Created / Modified / Erased, or new name on rename)
    fs::path       path;

    // Previous path for renames; empty otherwise
    fs::path       oldPath;

    // Time when the change was detected (not necessarily exact FS timestamp)
    std::chrono::steady_clock::time_point detectedAt;
};

// Simple snapshot-based file watcher.
// It periodically scans a directory tree and reports differences as events.
//
// Design:
//   - You specify a root directory and whether to watch recursively.
//   - Each call to update() computes differences vs the last snapshot.
//   - Events are stored internally and can be retrieved with poll_events().
class FileWatcher final {
public:
    FileWatcher() = default;
    explicit FileWatcher(fs::path root, bool recursive = true);

    FileWatcher(const FileWatcher&) = delete;
    FileWatcher& operator=(const FileWatcher&) = delete;

    FileWatcher(FileWatcher&&) noexcept = default;
    FileWatcher& operator=(FileWatcher&&) noexcept = default;

    ~FileWatcher() = default;

    // Set / change the root directory being watched.
    // Clears previous snapshot and events.
    void set_root(fs::path root, bool recursive = true);

    const fs::path& root() const { return m_root; }
    bool recursive() const { return m_recursive; }

    // Perform a scan and generate events for any detected changes.
    // Intended to be called periodically (e.g. once per frame, or via TaskScheduler).
    void update();

    // Retrieve and clear the accumulated events.
    std::vector<FileChangeEvent> poll_events();

    // Returns true if the watcher has a valid root directory.
    bool valid() const { return !m_root.empty(); }

private:
    using TimePoint = std::filesystem::file_time_type;

    struct EntryInfo {
        TimePoint lastWrite;
        std::uintmax_t size;
    };

    void clear_snapshot();
    void snapshot_current(std::unordered_map<std::string, EntryInfo>& outSnapshot);
    void compute_diff(const std::unordered_map<std::string, EntryInfo>& newSnapshot);

private:
    fs::path m_root;
    bool     m_recursive{true};

    // Map of path (string form, normalized) to entry info
    std::unordered_map<std::string, EntryInfo> m_snapshot;

    std::vector<FileChangeEvent> m_events;
};

} // namespace wave::engine::core::filesystem
