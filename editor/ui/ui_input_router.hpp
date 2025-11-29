#pragma once

#include "ui_context.hpp"
#include "panels/panel_manager.hpp"

#include <string>

namespace wave::editor::ui {

// Routes raw input (mouse, keys) onto the UI tree and panels.
// This is a logic layer – it does not talk to GLFW directly.
// The editor code should call these methods from the platform input callbacks.
class UIInputRouter final {
public:
    UIInputRouter() = default;
    ~UIInputRouter() = default;

    UIInputRouter(const UIInputRouter&) = delete;
    UIInputRouter& operator=(const UIInputRouter&) = delete;

    UIInputRouter(UIInputRouter&&) noexcept = default;
    UIInputRouter& operator=(UIInputRouter&&) noexcept = default;

    void set_sources(UIContext* context, PanelManager* panels);

    // Mouse --------------------------------------------------------------------

    void on_mouse_move(float x, float y);
    void on_mouse_button(int button, bool pressed);
    void on_scroll(float deltaX, float deltaY);

    // Keyboard (for future use) -----------------------------------------------

    void on_key(int key, int scancode, int action, int mods);

    // Queries ------------------------------------------------------------------

    // Returns the currently hovered panel, or nullptr.
    UIPanel* hovered_panel();
    const UIPanel* hovered_panel() const;

    // Returns the currently focused panel, or nullptr.
    UIPanel* focused_panel();
    const UIPanel* focused_panel() const;

    const std::string& hovered_panel_id() const { return m_hoveredPanelId; }
    const std::string& focused_panel_id() const { return m_focusedPanelId; }

private:
    void update_hover(float x, float y);
    void update_focus_from_hover();

private:
    UIContext*    m_context{nullptr};
    PanelManager* m_panels{nullptr};

    std::string m_hoveredPanelId;
    std::string m_focusedPanelId;

    float m_lastMouseX{0.0f};
    float m_lastMouseY{0.0f};

    bool m_mouseDownLeft{false};
    bool m_mouseDownRight{false};
};

} // namespace wave::editor::ui
