#pragma once

#include "../ui_panel.hpp"

#include <string>
#include <cstdint>

namespace wave::editor::ui {

// High level camera interaction modes for the editor viewport
enum class ViewportCameraMode {
    None,
    Orbit,
    Freefly,
    Pan
};

class ViewportPanel final : public UIPanel {
public:
    explicit ViewportPanel(std::string id = "viewport",
                           std::string title = "Viewport");

    ViewportPanel(const ViewportPanel&) = delete;
    ViewportPanel& operator=(const ViewportPanel&) = delete;

    ViewportPanel(ViewportPanel&&) noexcept = default;
    ViewportPanel& operator=(ViewportPanel&&) noexcept = default;

    ~ViewportPanel() override = default;

    // -------------------------------------------------------------------------
    // Viewport dimensions
    // -------------------------------------------------------------------------

    void set_viewport_size(std::uint32_t width, std::uint32_t height);
    std::uint32_t viewport_width() const { return m_viewportWidth; }
    std::uint32_t viewport_height() const { return m_viewportHeight; }

    bool size_changed() const { return m_sizeChanged; }
    void clear_size_flag() { m_sizeChanged = false; }

    // -------------------------------------------------------------------------
    // Camera interaction
    // -------------------------------------------------------------------------

    void begin_interaction(ViewportCameraMode mode);
    void end_interaction();

    ViewportCameraMode interaction_mode() const { return m_cameraMode; }
    bool interacting() const { return m_interacting; }

    void set_last_mouse(float x, float y);
    float last_mouse_x() const { return m_lastMouseX; }
    float last_mouse_y() const { return m_lastMouseY; }

    // Optional editor camera override
    void set_has_focus(bool focus) { m_hasFocus = focus; }
    bool has_focus() const { return m_hasFocus; }

    // -------------------------------------------------------------------------
    // Rendering interface bridge (no Vulkan code here)
    // -------------------------------------------------------------------------

    // The editor renderer will call this to update what the viewport should show.
    void set_render_texture_id(std::uint64_t id) { m_renderTextureId = id; }
    std::uint64_t render_texture_id() const { return m_renderTextureId; }

private:
    std::uint32_t m_viewportWidth{0};
    std::uint32_t m_viewportHeight{0};
    bool          m_sizeChanged{false};

    bool                m_interacting{false};
    ViewportCameraMode  m_cameraMode{ViewportCameraMode::None};

    float m_lastMouseX{0.0f};
    float m_lastMouseY{0.0f};

    bool m_hasFocus{false};

    // Abstract “texture handle” for UI renderer
    // e.g. ImGui image, custom draw surface, Vulkan-to-UI bridge ID, etc.
    std::uint64_t m_renderTextureId{0};
};

} // namespace wave::editor::ui
