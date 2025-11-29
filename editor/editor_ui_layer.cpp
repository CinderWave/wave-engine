#include "editor_ui_layer.hpp"

namespace wave::editor {

using namespace wave::editor::ui;

// -----------------------------------------------------------------------------
// Initialization / resize
// -----------------------------------------------------------------------------

void EditorUILayer::initialize(float windowWidth, float windowHeight) {
    m_windowWidth  = windowWidth;
    m_windowHeight = windowHeight;

    // Build initial UI layout + default panels
    m_editorUI.initialize(windowWidth, windowHeight);

    // Initial docking layout
    m_dockingLayout.rebuild(m_editorUI.context(), m_editorUI.panels());

    // Hook input router to current UI sources
    m_inputRouter.set_sources(&m_editorUI.context(), &m_editorUI.panels());
}

void EditorUILayer::on_resize(float windowWidth, float windowHeight) {
    m_windowWidth  = windowWidth;
    m_windowHeight = windowHeight;

    // Let EditorUI rebuild its root + docks
    m_editorUI.on_resize(windowWidth, windowHeight);

    // Rebuild docking layout for the new size
    m_dockingLayout.rebuild(m_editorUI.context(), m_editorUI.panels());
}

// -----------------------------------------------------------------------------
// Input forwarding
// -----------------------------------------------------------------------------

void EditorUILayer::on_mouse_move(float x, float y) {
    m_inputRouter.on_mouse_move(x, y);
}

void EditorUILayer::on_mouse_button(int button, bool pressed) {
    m_inputRouter.on_mouse_button(button, pressed);
}

void EditorUILayer::on_scroll(float deltaX, float deltaY) {
    m_inputRouter.on_scroll(deltaX, deltaY);
}

void EditorUILayer::on_key(int key, int scancode, int action, int mods) {
    m_inputRouter.on_key(key, scancode, action, mods);
}

// -----------------------------------------------------------------------------
// Layout / panels
// -----------------------------------------------------------------------------

void EditorUILayer::rebuild_layout() {
    m_dockingLayout.rebuild(m_editorUI.context(), m_editorUI.panels());
}

// -----------------------------------------------------------------------------
// Rendering
// -----------------------------------------------------------------------------

void EditorUILayer::build_draw_list() {
    m_uiRenderer.build_draw_list(m_editorUI, m_drawList);
}

} // namespace wave::editor
