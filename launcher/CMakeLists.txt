# Launcher executable

file(GLOB_RECURSE WAVE_LAUNCHER_SOURCES
    app/*.cpp
    core/*.cpp
    drm/*.cpp
)

add_executable(wave_launcher
    ${WAVE_LAUNCHER_SOURCES}
)

target_compile_features(wave_launcher PRIVATE cxx_std_20)

if (MSVC)
    target_compile_options(wave_launcher PRIVATE /W4 /permissive-)
else()
    target_compile_options(wave_launcher PRIVATE -Wall -Wextra -Wpedantic)
endif()
