#include "editor_ui.hpp"

namespace wave::editor::ui {

// -----------------------------------------------------------------------------
// Public API
// -----------------------------------------------------------------------------

void EditorUI::initialize(float width, float height) {
    m_width  = width;
    m_height = height;

    build_root_layout(width, height);
    create_default_panels();

    // Final layout pass
    Rect screenRect{};
    screenRect.x      = 0.0f;
    screenRect.y      = 0.0f;
    screenRect.width  = width;
    screenRect.height = height;

    m_context.update_layout(screenRect);
}

void EditorUI::on_resize(float width, float height) {
    m_width  = width;
    m_height = height;

    build_root_layout(width, height);

    Rect screenRect{};
    screenRect.x      = 0.0f;
    screenRect.y      = 0.0f;
    screenRect.width  = width;
    screenRect.height = height;

    m_context.update_layout(screenRect);
}

// -----------------------------------------------------------------------------
// Internal helpers
// -----------------------------------------------------------------------------

void EditorUI::build_root_layout(float width, float height) {
    // Root fills the entire window
    Rect rootRect{};
    rootRect.x      = 0.0f;
    rootRect.y      = 0.0f;
    rootRect.width  = width;
    rootRect.height = height;

    auto root = std::make_unique<UIElement>("root");
    root->set_anchor(Anchor::TopLeft);
    root->set_local_rect(rootRect);
    root->set_layout_mode(LayoutMode::Horizontal);

    // Left dock area (for things like hierarchy / inspector later)
    auto leftDock = std::make_unique<UIElement>("dock_left");
    {
        Rect r{};
        r.x      = 0.0f;
        r.y      = 0.0f;
        r.width  = width * 0.22f;      // ~22 percent of width
        r.height = height;
        leftDock->set_local_rect(r);
        leftDock->set_anchor(Anchor::TopLeft);
        leftDock->set_layout_mode(LayoutMode::Vertical);
    }

    // Center dock area (viewport, main content)
    auto centerDock = std::make_unique<UIElement>("dock_center");
    {
        Rect r{};
        r.x      = width * 0.22f;
        r.y      = 0.0f;
        r.width  = width * 0.56f;      // ~56 percent
        r.height = height;
        centerDock->set_local_rect(r);
        centerDock->set_anchor(Anchor::TopLeft);
        centerDock->set_layout_mode(LayoutMode::Vertical);
    }

    // Right dock area (for future inspector / profiler)
    auto rightDock = std::make_unique<UIElement>("dock_right");
    {
        Rect r{};
        r.x      = width * 0.78f;
        r.y      = 0.0f;
        r.width  = width * 0.22f;
        r.height = height;
        rightDock->set_local_rect(r);
        rightDock->set_anchor(Anchor::TopLeft);
        rightDock->set_layout_mode(LayoutMode::Vertical);
    }

    // Attach docks to root
    root->add_child(std::move(leftDock));
    root->add_child(std::move(centerDock));
    root->add_child(std::move(rightDock));

    m_context.set_root(std::move(root));
}

void EditorUI::create_default_panels() {
    // Console on the left
    {
        auto console = std::make_unique<ConsolePanel>("console", "Console");
        console->set_closable(true);
        console->set_movable(true);
        m_panelManager.register_panel(std::move(console), DockSlot::Left);
    }

    // Viewport in the center
    {
        auto viewport = std::make_unique<ViewportPanel>("viewport", "Viewport");
        viewport->set_closable(false);
        viewport->set_movable(true);
        m_panelManager.register_panel(std::move(viewport), DockSlot::Center);
    }

    // Additional panels (resource browser, stats, etc.) can be registered here later.
}

} // namespace wave::editor::ui
