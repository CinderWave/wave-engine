#include "ui_input_router.hpp"

#include "panels/ui_panel.hpp"

namespace wave::editor::ui {

// -----------------------------------------------------------------------------
// Setup
// -----------------------------------------------------------------------------

void UIInputRouter::set_sources(UIContext* context, PanelManager* panels) {
    m_context = context;
    m_panels  = panels;
}

// -----------------------------------------------------------------------------
// Mouse
// -----------------------------------------------------------------------------

void UIInputRouter::on_mouse_move(float x, float y) {
    m_lastMouseX = x;
    m_lastMouseY = y;

    update_hover(x, y);
}

void UIInputRouter::on_mouse_button(int button, bool pressed) {
    if (button == 0) { // left
        m_mouseDownLeft = pressed;
        if (pressed) {
            update_focus_from_hover();
        }
    } else if (button == 1) { // right
        m_mouseDownRight = pressed;
        if (pressed) {
            update_focus_from_hover();
        }
    }

    // Future: route mouse button to focused panel if needed.
}

void UIInputRouter::on_scroll(float /*deltaX*/, float /*deltaY*/) {
    // Placeholder for future per-panel scroll behaviour.
}

// -----------------------------------------------------------------------------
// Keyboard
// -----------------------------------------------------------------------------

void UIInputRouter::on_key(int /*key*/, int /*scancode*/, int /*action*/, int /*mods*/) {
    // Placeholder for routing keyboard events to the focused panel.
}

// -----------------------------------------------------------------------------
// Queries
// -----------------------------------------------------------------------------

UIPanel* UIInputRouter::hovered_panel() {
    if (!m_panels || m_hoveredPanelId.empty()) {
        return nullptr;
    }
    return m_panels->find_panel(m_hoveredPanelId);
}

const UIPanel* UIInputRouter::hovered_panel() const {
    if (!m_panels || m_hoveredPanelId.empty()) {
        return nullptr;
    }
    return m_panels->find_panel(m_hoveredPanelId);
}

UIPanel* UIInputRouter::focused_panel() {
    if (!m_panels || m_focusedPanelId.empty()) {
        return nullptr;
    }
    return m_panels->find_panel(m_focusedPanelId);
}

const UIPanel* UIInputRouter::focused_panel() const {
    if (!m_panels || m_focusedPanelId.empty()) {
        return nullptr;
    }
    return m_panels->find_panel(m_focusedPanelId);
}

// -----------------------------------------------------------------------------
// Internal helpers
// -----------------------------------------------------------------------------

void UIInputRouter::update_hover(float x, float y) {
    if (!m_context) {
        m_hoveredPanelId.clear();
        return;
    }

    UIElement* hit = m_context->hit_test(x, y);
    if (!hit) {
        m_hoveredPanelId.clear();
        return;
    }

    // The hit element has an id; see if it corresponds to a panel.
    const std::string& id = hit->id();
    if (!m_panels) {
        m_hoveredPanelId = id;
        return;
    }

    if (UIPanel* panel = m_panels->find_panel(id)) {
        m_hoveredPanelId = panel->id();
        return;
    }

    // If it's not a panel, clear or keep previous depending on taste.
    m_hoveredPanelId.clear();
}

void UIInputRouter::update_focus_from_hover() {
    if (!m_panels) {
        return;
    }

    if (m_hoveredPanelId.empty()) {
        return;
    }

    if (UIPanel* panel = m_panels->find_panel(m_hoveredPanelId)) {
        m_focusedPanelId = panel->id();
    }
}

} // namespace wave::editor::ui
