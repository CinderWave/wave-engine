#pragma once

#include <string_view>

#include "engine/core/logging/log.hpp"

namespace wave::engine::core::debug
{
    // Called internally by the assert macros.
    // Logs the failure and aborts the program.
    [[noreturn]] void handle_assert_failure(std::string_view expr,
                                            std::string_view message,
                                            std::string_view file,
                                            int line) noexcept;

} // namespace wave::engine::core::debug

// -----------------------------------------------------------------------------
// Assertion macros
// -----------------------------------------------------------------------------
//
// Usage:
//   WAVE_ASSERT(ptr != nullptr);
//   WAVE_ASSERT_MSG(count > 0, "count must be positive");
//
// In debug builds, a failing assert will:
//   - Log a CRITICAL entry with expression, file, line, and optional message.
//   - Call std::abort().
//
// In release builds (when NDEBUG is defined), they compile out.

#ifndef NDEBUG

    #define WAVE_ASSERT(expr)                                                       \
        do                                                                          \
        {                                                                           \
            if (!(expr))                                                            \
            {                                                                       \
                ::wave::engine::core::debug::handle_assert_failure(                 \
                    #expr, "", __FILE__, __LINE__);                                 \
            }                                                                       \
        } while (false)

    #define WAVE_ASSERT_MSG(expr, msg)                                              \
        do                                                                          \
        {                                                                           \
            if (!(expr))                                                            \
            {                                                                       \
                ::wave::engine::core::debug::handle_assert_failure(                 \
                    #expr, msg, __FILE__, __LINE__);                                \
            }                                                                       \
        } while (false)

#else // NDEBUG

    #define WAVE_ASSERT(expr)       do { (void)sizeof(expr); } while (false)
    #define WAVE_ASSERT_MSG(e, msg) do { (void)sizeof(e); (void)sizeof(msg); } while (false)

#endif
