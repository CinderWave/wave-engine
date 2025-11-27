#pragma once

#include <string>
#include <string_view>
#include <unordered_map>
#include <optional>

namespace wave::engine::core::config
{
    class Config
    {
    public:
        Config() = default;

        // Load from a file on disk. Simple format:
        //   key = value
        //   # comment lines start with '#'
        //
        // Whitespace around keys and values is trimmed.
        bool load_from_file(const std::string& path);

        // Clear all currently loaded keys.
        void clear() noexcept;

        // Raw access
        [[nodiscard]] bool has(std::string_view key) const noexcept;
        [[nodiscard]] std::optional<std::string> get(std::string_view key) const;

        // Typed helpers with default values
        [[nodiscard]] std::string get_string(std::string_view key,
                                             std::string default_value = {}) const;

        [[nodiscard]] int get_int(std::string_view key,
                                  int default_value = 0) const;

        [[nodiscard]] bool get_bool(std::string_view key,
                                    bool default_value = false) const;

        [[nodiscard]] float get_float(std::string_view key,
                                      float default_value = 0.0f) const;

    private:
        std::unordered_map<std::string, std::string> m_values;
    };

} // namespace wave::engine::core::config
