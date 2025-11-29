#include "config_system.hpp"

#include <fstream>
#include <sstream>
#include <charconv>
#include <algorithm>
#include <cctype>

namespace wave::engine::core::config {

// -----------------------------------------------------------------------------
// Helpers
// -----------------------------------------------------------------------------

namespace {

inline std::string trim_copy(std::string_view sv) {
    std::size_t start = 0;
    while (start < sv.size() && std::isspace(static_cast<unsigned char>(sv[start]))) {
        ++start;
    }

    std::size_t end = sv.size();
    while (end > start && std::isspace(static_cast<unsigned char>(sv[end - 1]))) {
        --end;
    }

    return std::string(sv.substr(start, end - start));
}

ConfigValue parse_value(std::string_view valueText) {
    std::string trimmed = trim_copy(valueText);
    if (trimmed.empty()) {
        return ConfigValue{};
    }

    // Boolean
    if (trimmed == "true" || trimmed == "True") {
        return ConfigValue(true);
    }
    if (trimmed == "false" || trimmed == "False") {
        return ConfigValue(false);
    }

    // String (quoted)
    if (trimmed.size() >= 2 && trimmed.front() == '"' && trimmed.back() == '"') {
        std::string inner = trimmed.substr(1, trimmed.size() - 2);
        // Basic unescape support can be added later
        return ConfigValue(std::move(inner));
    }

    // Try integer
    {
        std::int64_t intVal = 0;
        auto first = trimmed.data();
        auto last  = trimmed.data() + trimmed.size();
        auto res   = std::from_chars(first, last, intVal);
        if (res.ec == std::errc() && res.ptr == last) {
            return ConfigValue(intVal);
        }
    }

    // Try double
    try {
        std::size_t idx = 0;
        double d = std::stod(trimmed, &idx);
        if (idx == trimmed.size()) {
            return ConfigValue(d);
        }
    } catch (...) {
        // fall through to string
    }

    // Fallback: raw string
    return ConfigValue(std::move(trimmed));
}

} // namespace

// -----------------------------------------------------------------------------
// ConfigValue
// -----------------------------------------------------------------------------

bool ConfigValue::as_bool(bool defaultValue) const {
    if (auto p = std::get_if<bool>(&m_storage)) {
        return *p;
    }
    if (auto p = std::get_if<std::int64_t>(&m_storage)) {
        return *p != 0;
    }
    if (auto p = std::get_if<double>(&m_storage)) {
        return *p != 0.0;
    }
    if (auto p = std::get_if<std::string>(&m_storage)) {
        if (*p == "true" || *p == "True" || *p == "1") {
            return true;
        }
        if (*p == "false" || *p == "False" || *p == "0") {
            return false;
        }
    }
    return defaultValue;
}

std::int64_t ConfigValue::as_int(std::int64_t defaultValue) const {
    if (auto p = std::get_if<std::int64_t>(&m_storage)) {
        return *p;
    }
    if (auto p = std::get_if<double>(&m_storage)) {
        return static_cast<std::int64_t>(*p);
    }
    if (auto p = std::get_if<bool>(&m_storage)) {
        return *p ? 1 : 0;
    }
    if (auto p = std::get_if<std::string>(&m_storage)) {
        std::int64_t val = 0;
        auto first = p->data();
        auto last  = p->data() + p->size();
        auto res   = std::from_chars(first, last, val);
        if (res.ec == std::errc() && res.ptr == last) {
            return val;
        }
    }
    return defaultValue;
}

double ConfigValue::as_double(double defaultValue) const {
    if (auto p = std::get_if<double>(&m_storage)) {
        return *p;
    }
    if (auto p = std::get_if<std::int64_t>(&m_storage)) {
        return static_cast<double>(*p);
    }
    if (auto p = std::get_if<bool>(&m_storage)) {
        return *p ? 1.0 : 0.0;
    }
    if (auto p = std::get_if<std::string>(&m_storage)) {
        try {
            std::size_t idx = 0;
            double d = std::stod(*p, &idx);
            if (idx == p->size()) {
                return d;
            }
        } catch (...) {
        }
    }
    return defaultValue;
}

std::string ConfigValue::as_string(const std::string& defaultValue) const {
    if (auto p = std::get_if<std::string>(&m_storage)) {
        return *p;
    }
    if (auto p = std::get_if<bool>(&m_storage)) {
        return *p ? "true" : "false";
    }
    if (auto p = std::get_if<std::int64_t>(&m_storage)) {
        return std::to_string(*p);
    }
    if (auto p = std::get_if<double>(&m_storage)) {
        return std::to_string(*p);
    }
    return defaultValue;
}

// -----------------------------------------------------------------------------
// ConfigTable
// -----------------------------------------------------------------------------

void ConfigTable::set(std::string key, ConfigValue value) {
    m_entries[std::move(key)] = std::move(value);
}

bool ConfigTable::has(std::string_view key) const {
    return m_entries.find(std::string(key)) != m_entries.end();
}

const ConfigValue* ConfigTable::get(std::string_view key) const {
    auto it = m_entries.find(std::string(key));
    if (it == m_entries.end()) {
        return nullptr;
    }
    return &it->second;
}

bool ConfigTable::get_bool(std::string_view key, bool defaultValue) const {
    if (const ConfigValue* v = get(key)) {
        return v->as_bool(defaultValue);
    }
    return defaultValue;
}

std::int64_t ConfigTable::get_int(std::string_view key, std::int64_t defaultValue) const {
    if (const ConfigValue* v = get(key)) {
        return v->as_int(defaultValue);
    }
    return defaultValue;
}

double ConfigTable::get_double(std::string_view key, double defaultValue) const {
    if (const ConfigValue* v = get(key)) {
        return v->as_double(defaultValue);
    }
    return defaultValue;
}

std::string ConfigTable::get_string(std::string_view key, const std::string& defaultValue) const {
    if (const ConfigValue* v = get(key)) {
        return v->as_string(defaultValue);
    }
    return defaultValue;
}

// -----------------------------------------------------------------------------
// ConfigSystem
// -----------------------------------------------------------------------------

bool ConfigSystem::load_from_file(const fs::path& path, ConfigFormat format) {
    // For now, all formats use the same simple parser.
    (void)format;
    return load_simple_format(path);
}

bool ConfigSystem::load_simple_format(const fs::path& path) {
    std::ifstream file(path);
    if (!file) {
        return false;
    }

    m_table = ConfigTable{};

    std::string line;
    while (std::getline(file, line)) {
        std::string_view sv(line);

        // Strip comments starting with # or //
        if (auto pos = sv.find('#'); pos != std::string_view::npos) {
            sv = sv.substr(0, pos);
        } else if (auto pos2 = sv.find("//"); pos2 != std::string_view::npos) {
            sv = sv.substr(0, pos2);
        }

        const std::string trimmedLine = trim_copy(sv);
        if (trimmedLine.empty()) {
            continue;
        }

        std::string_view tv(trimmedLine);

        // Support "key = value" and "key: value"
        std::size_t sepPos = tv.find('=');
        if (sepPos == std::string_view::npos) {
            sepPos = tv.find(':');
        }
        if (sepPos == std::string_view::npos) {
            continue;
        }

        std::string key = trim_copy(tv.substr(0, sepPos));
        std::string valueText = trim_copy(tv.substr(sepPos + 1));

        if (key.empty()) {
            continue;
        }

        ConfigValue value = parse_value(valueText);
        m_table.set(std::move(key), std::move(value));
    }

    return true;
}

// Typed passthrough helpers ---------------------------------------------------

bool ConfigSystem::get_bool(std::string_view key, bool defaultValue) const {
    return m_table.get_bool(key, defaultValue);
}

std::int64_t ConfigSystem::get_int(std::string_view key, std::int64_t defaultValue) const {
    return m_table.get_int(key, defaultValue);
}

double ConfigSystem::get_double(std::string_view key, double defaultValue) const {
    return m_table.get_double(key, defaultValue);
}

std::string ConfigSystem::get_string(std::string_view key, const std::string& defaultValue) const {
    return m_table.get_string(key, defaultValue);
}

} // namespace wave::engine::core::config
