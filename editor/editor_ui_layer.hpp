#pragma once

#include "ui/editor_ui.hpp"
#include "ui/docking/docking_layout.hpp"
#include "ui/ui_input_router.hpp"
#include "ui/render/editor_ui_renderer.hpp"
#include "ui/render/ui_draw_list.hpp"

namespace wave::editor {

// High level façade that bundles all editor UI subsystems:
//
//   - EditorUI          (layout + panels)
//   - DockingLayout     (places panels into docks)
//   - UIInputRouter     (hover/focus from mouse / keys)
//   - EditorUIRenderer  (builds draw list)
//   - UIDrawList        (per frame UI commands)
//
// The EditorApp only needs to talk to this layer instead of
// wiring each UI subsystem manually.
class EditorUILayer final {
public:
    EditorUILayer() = default;
    ~EditorUILayer() = default;

    EditorUILayer(const EditorUILayer&) = delete;
    EditorUILayer& operator=(const EditorUILayer&) = delete;

    EditorUILayer(EditorUILayer&&) noexcept = default;
    EditorUILayer& operator=(EditorUILayer&&) noexcept = default;

    // Must be called after the window is created and you know its size.
    void initialize(float windowWidth, float windowHeight);

    // Call from the window resize callback.
    void on_resize(float windowWidth, float windowHeight);

    // Input forwarding (call from platform input layer) -----------------------

    void on_mouse_move(float x, float y);
    void on_mouse_button(int button, bool pressed);
    void on_scroll(float deltaX, float deltaY);

    void on_key(int key, int scancode, int action, int mods);

    // Layout / panel changes ---------------------------------------------------

    // Rebuild docking layout after:
    //   - panel visibility change
    //   - new panel registration
    //   - docking slot changes
    void rebuild_layout();

    // Rendering ---------------------------------------------------------------

    // Build the UI draw list for this frame.
    // Call once per frame before handing drawList to the renderer backend.
    void build_draw_list();

    const ui::UIDrawList& draw_list() const { return m_drawList; }

    // Access to underlying systems if the editor needs deeper control ---------

    ui::EditorUI&          editor_ui()       { return m_editorUI; }
    const ui::EditorUI&    editor_ui() const { return m_editorUI; }

    ui::UIInputRouter&     input_router()       { return m_inputRouter; }
    const ui::UIInputRouter& input_router() const { return m_inputRouter; }

    ui::PanelManager&      panels()       { return m_editorUI.panels(); }
    const ui::PanelManager& panels() const { return m_editorUI.panels(); }

private:
    ui::EditorUI        m_editorUI;
    ui::DockingLayout   m_dockingLayout;
    ui::UIInputRouter   m_inputRouter;
    ui::EditorUIRenderer m_uiRenderer;
    ui::UIDrawList      m_drawList;

    float m_windowWidth{0.0f};
    float m_windowHeight{0.0f};
};

} // namespace wave::editor
