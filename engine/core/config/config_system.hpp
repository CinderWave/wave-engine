#pragma once

#include <string>
#include <string_view>
#include <unordered_map>
#include <variant>
#include <filesystem>
#include <cstdint>

namespace wave::engine::core::config {

namespace fs = std::filesystem;

// Supported config formats for future expansion.
// For now, the implementation uses a simple key = value format
// regardless of the specified format, but the API is ready to
// plug in JSON / TOML parsers later.
enum class ConfigFormat {
    Auto,
    Simple,   // simple key = value format
    Json,
    Toml
};

// Variant value type for configuration entries.
class ConfigValue {
public:
    using Storage = std::variant<std::monostate, bool, std::int64_t, double, std::string>;

    ConfigValue() = default;

    ConfigValue(bool v)               : m_storage(v) {}
    ConfigValue(std::int64_t v)       : m_storage(v) {}
    ConfigValue(double v)             : m_storage(v) {}
    ConfigValue(std::string v)        : m_storage(std::move(v)) {}
    ConfigValue(const char* v)        : m_storage(std::string(v)) {}

    const Storage& storage() const { return m_storage; }

    bool is_null()   const { return std::holds_alternative<std::monostate>(m_storage); }
    bool is_bool()   const { return std::holds_alternative<bool>(m_storage); }
    bool is_int()    const { return std::holds_alternative<std::int64_t>(m_storage); }
    bool is_double() const { return std::holds_alternative<double>(m_storage); }
    bool is_string() const { return std::holds_alternative<std::string>(m_storage); }

    bool        as_bool(bool defaultValue = false) const;
    std::int64_t as_int(std::int64_t defaultValue = 0) const;
    double      as_double(double defaultValue = 0.0) const;
    std::string as_string(const std::string& defaultValue = {}) const;

private:
    Storage m_storage{};
};

// Flat config table using dotted keys (e.g. "render.width").
// Internally just a string → ConfigValue map.
class ConfigTable {
public:
    ConfigTable() = default;

    void set(std::string key, ConfigValue value);
    bool has(std::string_view key) const;

    const ConfigValue* get(std::string_view key) const;

    // Typed helpers -----------------------------------------------------------

    bool        get_bool(std::string_view key, bool defaultValue = false) const;
    std::int64_t get_int(std::string_view key, std::int64_t defaultValue = 0) const;
    double      get_double(std::string_view key, double defaultValue = 0.0) const;
    std::string get_string(std::string_view key, const std::string& defaultValue = {}) const;

    const std::unordered_map<std::string, ConfigValue>& entries() const { return m_entries; }

private:
    std::unordered_map<std::string, ConfigValue> m_entries;
};

// Engine-level config system that owns a single ConfigTable and
// provides file loading and access helpers.
class ConfigSystem final {
public:
    ConfigSystem() = default;
    ~ConfigSystem() = default;

    ConfigSystem(const ConfigSystem&) = delete;
    ConfigSystem& operator=(const ConfigSystem&) = delete;

    ConfigSystem(ConfigSystem&&) noexcept = default;
    ConfigSystem& operator=(ConfigSystem&&) noexcept = default;

    // Load configuration from a file.
    //
    // Current implementation:
    //   - Reads text file
    //   - Parses a simple "key = value" format
    //
    // Future: use ConfigFormat::Json / ConfigFormat::Toml with
    // proper parsers without changing this API.
    bool load_from_file(const fs::path& path, ConfigFormat format = ConfigFormat::Auto);

    const ConfigTable& table() const { return m_table; }
          ConfigTable& table()       { return m_table; }

    // Convenience typed getters ----------------------------------------------

    bool        get_bool(std::string_view key, bool defaultValue = false) const;
    std::int64_t get_int(std::string_view key, std::int64_t defaultValue = 0) const;
    double      get_double(std::string_view key, double defaultValue = 0.0) const;
    std::string get_string(std::string_view key, const std::string& defaultValue = {}) const;

private:
    bool load_simple_format(const fs::path& path);

private:
    ConfigTable m_table;
};

} // namespace wave::engine::core::config
