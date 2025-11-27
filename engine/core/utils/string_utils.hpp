#pragma once

#include <string>
#include <string_view>

namespace wave::engine::core::utils
{
    // Trim whitespace (space, tab, CR, LF) from both ends.
    std::string trim(std::string_view input);

    // Convert ASCII characters to lowercase.
    std::string to_lower(std::string_view input);

} // namespace wave::engine::core::utils
