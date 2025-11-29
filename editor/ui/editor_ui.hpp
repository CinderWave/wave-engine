#pragma once

#include "ui_context.hpp"
#include "panels/panel_manager.hpp"
#include "panels/console/console_panel.hpp"
#include "panels/viewport/viewport_panel.hpp"

namespace wave::editor::ui {

// Central coordinator for the editor's UI:
//  - Owns the UIContext (layout tree)
//  - Owns the PanelManager (panels + docking intent)
//  - Builds a simple default layout and panels
class EditorUI final {
public:
    EditorUI() = default;
    ~EditorUI() = default;

    EditorUI(const EditorUI&) = delete;
    EditorUI& operator=(const EditorUI&) = delete;

    EditorUI(EditorUI&&) noexcept = default;
    EditorUI& operator=(EditorUI&&) noexcept = default;

    // Build the initial layout and default panels for a given window size.
    void initialize(float width, float height);

    // Resize notification from the editor window.
    // Rebuilds the root rect and updates layout.
    void on_resize(float width, float height);

    // Access to subsystems -----------------------------------------------------

    UIContext&      context()       { return m_context; }
    const UIContext& context() const { return m_context; }

    PanelManager&      panels()       { return m_panelManager; }
    const PanelManager& panels() const { return m_panelManager; }

private:
    void build_root_layout(float width, float height);
    void create_default_panels();

private:
    UIContext    m_context;
    PanelManager m_panelManager;

    float m_width{0.0f};
    float m_height{0.0f};
};

} // namespace wave::editor::ui
