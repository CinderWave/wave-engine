# Engine core library

set(WAVE_ENGINE_CORE_SOURCES
    core/engine_core.cpp
)

add_library(wave_engine_core STATIC
    ${WAVE_ENGINE_CORE_SOURCES}
)

target_include_directories(wave_engine_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/core
)

target_compile_features(wave_engine_core PRIVATE cxx_std_20)

if (MSVC)
    target_compile_options(wave_engine_core PRIVATE /W4 /permissive-)
else()
    target_compile_options(wave_engine_core PRIVATE -Wall -Wextra -Wpedantic)
endif()
