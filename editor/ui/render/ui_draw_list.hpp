#pragma once

#include "ui_draw_command.hpp"
#include "../../ui/ui_context.hpp"

#include <vector>

namespace wave::editor::ui {

// Collects draw commands for a single UI frame.
// Called by the UI rendering pass in the editor pipeline.
class UIDrawList final {
public:
    UIDrawList() = default;
    ~UIDrawList() = default;

    UIDrawList(const UIDrawList&) = delete;
    UIDrawList& operator=(const UIDrawList&) = delete;

    UIDrawList(UIDrawList&&) noexcept = default;
    UIDrawList& operator=(UIDrawList&&) noexcept = default;

    void clear();

    // Push raw draw commands ---------------------------------------------------

    void draw_rect(float x, float y, float w, float h,
                   float r, float g, float b, float a);

    void draw_text(float x, float y, const std::string& text,
                   float fontSize,
                   float r, float g, float b, float a);

    void draw_image(float x, float y, float w, float h,
                    std::uint64_t textureId);

    // High-level helpers -------------------------------------------------------

    void draw_panel_background(const UIElement& e);

    void draw_panel_titlebar(const UIElement& e,
                             const std::string& title);

    // Access -------------------------------------------------------------------

    const std::vector<UIDrawCommand>& commands() const { return m_commands; }

private:
    std::vector<UIDrawCommand> m_commands;
};

} // namespace wave::editor::ui
