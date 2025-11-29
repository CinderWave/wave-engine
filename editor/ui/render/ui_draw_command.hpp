#pragma once

#include <cstdint>
#include <string>

namespace wave::editor::ui {

// Basic shapes that the UI renderer can draw.
// This is backend independent; Vulkan, Metal, etc will implement the actual GPU work.
enum class UIDrawShape {
    Rect,
    Text,
    Image
};

// A single UI draw command.
struct UIDrawCommand {
    UIDrawShape shape{UIDrawShape::Rect};

    // Position and size
    float x{0.0f};
    float y{0.0f};
    float width{0.0f};
    float height{0.0f};

    // Color in RGBA (0–1). Backend will convert to appropriate format.
    float r{1.0f};
    float g{1.0f};
    float b{1.0f};
    float a{1.0f};

    // Optional text
    std::string text;
    float fontSize{14.0f};

    // Optional image / texture ID
    std::uint64_t textureId{0};
};

} // namespace wave::editor::ui
