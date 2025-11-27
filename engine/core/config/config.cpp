#include "engine/core/config/config.hpp"

#include <fstream>
#include <sstream>
#include <charconv>

#include "engine/core/utils/string_utils.hpp"

namespace wave::engine::core::config
{
    using wave::engine::core::utils::trim;
    using wave::engine::core::utils::to_lower;

    bool Config::load_from_file(const std::string& path)
    {
        std::ifstream file(path);
        if (!file)
            return false;

        m_values.clear();

        std::string line;
        while (std::getline(file, line))
        {
            // Strip comments
            auto hash_pos = line.find('#');
            if (hash_pos != std::string::npos)
                line.erase(hash_pos);

            line = trim(line);
            if (line.empty())
                continue;

            auto eq_pos = line.find('=');
            if (eq_pos == std::string::npos)
                continue; // ignore malformed lines

            std::string key   = trim(line.substr(0, eq_pos));
            std::string value = trim(line.substr(eq_pos + 1));

            if (!key.empty())
                m_values[key] = value;
        }

        return true;
    }

    void Config::clear() noexcept
    {
        m_values.clear();
    }

    bool Config::has(std::string_view key) const noexcept
    {
        return m_values.find(std::string(key)) != m_values.end();
    }

    std::optional<std::string> Config::get(std::string_view key) const
    {
        auto it = m_values.find(std::string(key));
        if (it == m_values.end())
            return std::nullopt;

        return it->second;
    }

    std::string Config::get_string(std::string_view key,
                                   std::string default_value) const
    {
        auto it = m_values.find(std::string(key));
        if (it == m_values.end())
            return default_value;

        return it->second;
    }

    int Config::get_int(std::string_view key, int default_value) const
    {
        auto it = m_values.find(std::string(key));
        if (it == m_values.end())
            return default_value;

        int value{};
        auto str   = it->second;
        auto* begin = str.data();
        auto* end   = str.data() + str.size();

        if (std::from_chars(begin, end, value).ec == std::errc{})
            return value;

        return default_value;
    }

    bool Config::get_bool(std::string_view key, bool default_value) const
    {
        auto it = m_values.find(std::string(key));
        if (it == m_values.end())
            return default_value;

        std::string value = to_lower(trim(it->second));

        if (value == "1" || value == "true" || value == "yes" || value == "on")
            return true;
        if (value == "0" || value == "false" || value == "no" || value == "off")
            return false;

        return default_value;
    }

    float Config::get_float(std::string_view key, float default_value) const
    {
        auto it = m_values.find(std::string(key));
        if (it == m_values.end())
            return default_value;

        float value{};
        auto str   = it->second;
        auto* begin = str.data();
        auto* end   = str.data() + str.size();

        if (std::from_chars(begin, end, value).ec == std::errc{})
            return value;

        return default_value;
    }

} // namespace wave::engine::core::config
