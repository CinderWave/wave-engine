#include "engine/core/utils/string_utils.hpp"

#include <algorithm>
#include <cctype>

namespace wave::engine::core::utils
{
    std::string trim(std::string_view input)
    {
        std::size_t begin = 0;
        std::size_t end   = input.size();

        while (begin < end &&
               (input[begin] == ' ' || input[begin] == '\t' ||
                input[begin] == '\r' || input[begin] == '\n'))
        {
            ++begin;
        }

        while (end > begin &&
               (input[end - 1] == ' ' || input[end - 1] == '\t' ||
                input[end - 1] == '\r' || input[end - 1] == '\n'))
        {
            --end;
        }

        if (begin == end)
            return {};

        return std::string(input.substr(begin, end - begin));
    }

    std::string to_lower(std::string_view input)
    {
        std::string result;
        result.reserve(input.size());

        std::transform(input.begin(), input.end(), std::back_inserter(result),
                       [](unsigned char c)
                       {
                           return static_cast<char>(std::tolower(c));
                       });

        return result;
    }

} // namespace wave::engine::core::utils
