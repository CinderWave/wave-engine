#pragma once

#include <string>
#include <string_view>
#include <filesystem>

namespace wave::engine::core::filesystem
{
    namespace fs = std::filesystem;

    // Lightweight filesystem helpers for engine / launcher / editor.
    // This does not try to be a full virtual filesystem; it is just a safe,
    // centralized place for common file operations.

    // Read the entire file as UTF-8 (or binary treated as bytes) into 'out'.
    // Returns true on success, false on any error.
    bool read_text_file(const fs::path& path, std::string& out) noexcept;

    // Write the given text to a file, overwriting any existing contents.
    // Creates parent directories if needed.
    // Returns true on success, false on any error.
    bool write_text_file(const fs::path& path, std::string_view text) noexcept;

    // Ensure a directory exists (recursively created if needed).
    // Returns true if the directory exists or was created successfully.
    bool ensure_directory(const fs::path& path) noexcept;

    // Remove a file if it exists.
    // Returns true if the file does not exist or was removed successfully.
    bool remove_file(const fs::path& path) noexcept;

    // Convenience: does a file exist and is it a regular file?
    bool file_exists(const fs::path& path) noexcept;

} // namespace wave::engine::core::filesystem
