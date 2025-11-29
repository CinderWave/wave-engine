#pragma once

//
// Global build configuration for Wave Engine.
//
// This header is the single place where we translate compiler / CMake
// definitions into engine-wide feature flags.
//
// Typical CMake options that might be defined:
//
//   -D WAVE_INTERNAL_BUILD=ON
//   -D WAVE_ENABLE_LOGGING=ON
//   -D WAVE_ENABLE_ASSERTS=ON
//

namespace wave::engine::core::build
{
    // Internal vs public build
    // ------------------------
    //
    // WAVE_INTERNAL_BUILD should be defined by the internal CMake project.
    // The public repo's CMake can omit it or explicitly set it to 0.

#if defined(WAVE_INTERNAL_BUILD) && WAVE_INTERNAL_BUILD
    constexpr bool kIsInternalBuild = true;
#else
    constexpr bool kIsInternalBuild = false;
#endif

    // Logging toggle
    // --------------
    //
    // Logging can be enabled or disabled at compile time.
    // Defaults:
    //   - Enabled in Debug / RelWithDebInfo
    //   - Can be disabled in Release for performance if desired.

#if defined(WAVE_ENABLE_LOGGING)
    constexpr bool kLoggingEnabled = (WAVE_ENABLE_LOGGING != 0);
#elif !defined(NDEBUG)
    constexpr bool kLoggingEnabled = true;
#else
    constexpr bool kLoggingEnabled = true; // change to false if you want silent Release
#endif

    // Assert toggle
    // -------------
    //
    // By default, asserts follow the standard NDEBUG convention:
    //   - Enabled in Debug
    //   - Disabled in Release
    // But can be forced on/off with WAVE_ENABLE_ASSERTS.

#if defined(WAVE_ENABLE_ASSERTS)
    constexpr bool kAssertsEnabled = (WAVE_ENABLE_ASSERTS != 0);
#elif !defined(NDEBUG)
    constexpr bool kAssertsEnabled = true;
#else
    constexpr bool kAssertsEnabled = false;
#endif

    // DRM behavior flags
    // ------------------
    //
    // Example of how we can branch DRM / dev features:
    // internal builds may allow dev licenses or skip strict checks,
    // while public builds are stricter.

    constexpr bool kDevFriendlyDrm =
        kIsInternalBuild; // internal builds get dev-friendly DRM by default

} // namespace wave::engine::core::build
