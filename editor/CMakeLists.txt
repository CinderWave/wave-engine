# Editor executable

file(GLOB_RECURSE WAVE_EDITOR_SOURCES
    app/entry/*.cpp
)

add_executable(wave_editor
    ${WAVE_EDITOR_SOURCES}
)

target_link_libraries(wave_editor
    PRIVATE
        wave_engine_core
)

target_compile_features(wave_editor PRIVATE cxx_std_20)

if (MSVC)
    target_compile_options(wave_editor PRIVATE /W4 /permissive-)
else()
    target_compile_options(wave_editor PRIVATE -Wall -Wextra -Wpedantic)
endif()
