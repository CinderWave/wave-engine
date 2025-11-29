#include "ui_draw_list.hpp"

namespace wave::editor::ui {

// -----------------------------------------------------------------------------
// Basic control
// -----------------------------------------------------------------------------

void UIDrawList::clear() {
    m_commands.clear();
}

// -----------------------------------------------------------------------------
// Primitive commands
// -----------------------------------------------------------------------------

void UIDrawList::draw_rect(float x, float y, float w, float h,
                           float r, float g, float b, float a) {
    UIDrawCommand cmd{};
    cmd.shape  = UIDrawShape::Rect;
    cmd.x      = x;
    cmd.y      = y;
    cmd.width  = w;
    cmd.height = h;
    cmd.r      = r;
    cmd.g      = g;
    cmd.b      = b;
    cmd.a      = a;
    m_commands.push_back(std::move(cmd));
}

void UIDrawList::draw_text(float x, float y, const std::string& text,
                           float fontSize,
                           float r, float g, float b, float a) {
    UIDrawCommand cmd{};
    cmd.shape    = UIDrawShape::Text;
    cmd.x        = x;
    cmd.y        = y;
    cmd.text     = text;
    cmd.fontSize = fontSize;
    cmd.r        = r;
    cmd.g        = g;
    cmd.b        = b;
    cmd.a        = a;
    m_commands.push_back(std::move(cmd));
}

void UIDrawList::draw_image(float x, float y, float w, float h,
                            std::uint64_t textureId) {
    UIDrawCommand cmd{};
    cmd.shape     = UIDrawShape::Image;
    cmd.x         = x;
    cmd.y         = y;
    cmd.width     = w;
    cmd.height    = h;
    cmd.textureId = textureId;
    m_commands.push_back(std::move(cmd));
}

// -----------------------------------------------------------------------------
// UI helpers
// -----------------------------------------------------------------------------

void UIDrawList::draw_panel_background(const UIElement& e) {
    const Rect& r = e.global_rect();

    // Simple dark panel background color
    draw_rect(
        r.x, r.y, r.width, r.height,
        0.12f, 0.12f, 0.12f, 1.0f
    );
}

void UIDrawList::draw_panel_titlebar(const UIElement& e,
                                     const std::string& title) {
    const Rect& r = e.global_rect();

    float barHeight = 26.0f;

    draw_rect(
        r.x, r.y, r.width, barHeight,
        0.18f, 0.18f, 0.18f, 1.0f
    );

    draw_text(
        r.x + 8.0f, r.y + 5.0f,
        title,
        14.0f,
        1.0f, 1.0f, 1.0f, 1.0f
    );
}

} // namespace wave::editor::ui
