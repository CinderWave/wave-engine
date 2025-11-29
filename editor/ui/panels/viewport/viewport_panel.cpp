#include "viewport_panel.hpp"

namespace wave::editor::ui {

// -----------------------------------------------------------------------------
// Construction
// -----------------------------------------------------------------------------

ViewportPanel::ViewportPanel(std::string id, std::string title)
    : UIPanel(std::move(id), std::move(title), PanelKind::Viewport) {}

// -----------------------------------------------------------------------------
// Viewport size
// -----------------------------------------------------------------------------

void ViewportPanel::set_viewport_size(std::uint32_t width, std::uint32_t height) {
    if (width != m_viewportWidth || height != m_viewportHeight) {
        m_viewportWidth  = width;
        m_viewportHeight = height;
        m_sizeChanged = true;
    }
}

// -----------------------------------------------------------------------------
// Interaction
// -----------------------------------------------------------------------------

void ViewportPanel::begin_interaction(ViewportCameraMode mode) {
    m_cameraMode = mode;
    m_interacting = (mode != ViewportCameraMode::None);
}

void ViewportPanel::end_interaction() {
    m_cameraMode = ViewportCameraMode::None;
    m_interacting = false;
}

void ViewportPanel::set_last_mouse(float x, float y) {
    m_lastMouseX = x;
    m_lastMouseY = y;
}

} // namespace wave::editor::ui
