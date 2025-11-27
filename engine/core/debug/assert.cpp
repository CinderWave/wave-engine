#include "engine/core/debug/assert.hpp"

#include <cstdlib> // std::abort
#include <string>

namespace wave::engine::core::debug
{
    void handle_assert_failure(std::string_view expr,
                               std::string_view message,
                               std::string_view file,
                               int line) noexcept
    {
        // Build a readable message for the logger.
        std::string full_msg;
        full_msg.reserve(256);

        full_msg += "Assertion failed: ";
        full_msg += expr;
        full_msg += "  at ";
        full_msg += file;
        full_msg += ':';
        full_msg += std::to_string(line);

        if (!message.empty())
        {
            full_msg += "  |  ";
            full_msg += message;
        }

        WAVE_LOG_CRITICAL(full_msg);

        // Last resort: abort the process so the failure is visible.
        std::abort();
    }

} // namespace wave::engine::core::debug
