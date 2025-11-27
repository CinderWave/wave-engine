#include "engine/core/filesystem/file_system.hpp"

#include <fstream>

namespace wave::engine::core::filesystem
{
    bool read_text_file(const fs::path& path, std::string& out) noexcept
    {
        out.clear();

        std::ifstream file(path, std::ios::in | std::ios::binary);
        if (!file)
            return false;

        file.seekg(0, std::ios::end);
        const auto size = file.tellg();
        if (size < 0)
            return false;

        out.resize(static_cast<std::size_t>(size));
        file.seekg(0, std::ios::beg);

        if (!file.read(out.data(), size))
        {
            out.clear();
            return false;
        }

        return true;
    }

    bool write_text_file(const fs::path& path, std::string_view text) noexcept
    {
        try
        {
            if (auto parent = path.parent_path(); !parent.empty())
            {
                std::error_code ec;
                fs::create_directories(parent, ec);
                if (ec)
                    return false;
            }

            std::ofstream file(path, std::ios::out | std::ios::binary | std::ios::trunc);
            if (!file)
                return false;

            file.write(text.data(), static_cast<std::streamsize>(text.size()));
            if (!file)
                return false;

            file.flush();
            return static_cast<bool>(file);
        }
        catch (...)
        {
            return false;
        }
    }

    bool ensure_directory(const fs::path& path) noexcept
    {
        try
        {
            std::error_code ec;
            fs::create_directories(path, ec);
            if (ec)
                return false;

            return fs::is_directory(path);
        }
        catch (...)
        {
            return false;
        }
    }

    bool remove_file(const fs::path& path) noexcept
    {
        try
        {
            std::error_code ec;
            if (!fs::exists(path, ec))
                return true; // already gone or cannot be seen

            if (!fs::is_regular_file(path, ec))
                return false;

            fs::remove(path, ec);
            return !ec;
        }
        catch (...)
        {
            return false;
        }
    }

    bool file_exists(const fs::path& path) noexcept
    {
        try
        {
            std::error_code ec;
            return fs::exists(path, ec) && fs::is_regular_file(path, ec);
        }
        catch (...)
        {
            return false;
        }
    }

} // namespace wave::engine::core::filesystem
