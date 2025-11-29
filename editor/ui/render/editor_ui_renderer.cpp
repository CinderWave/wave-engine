#include "editor_ui_renderer.hpp"

#include "../panels/ui_panel.hpp"
#include "../panels/viewport/viewport_panel.hpp"
#include "../panels/panel_manager.hpp"

namespace wave::editor::ui {

void EditorUIRenderer::build_draw_list(const EditorUI& editorUI, UIDrawList& drawList) {
    drawList.clear();

    const PanelManager& panelManager = editorUI.panels();
    const auto& entries = panelManager.panels();

    for (const PanelEntry& entry : entries) {
        if (!entry.panel || !entry.visible) {
            continue;
        }

        draw_panel(drawList, editorUI, *entry.panel);
    }
}

void EditorUIRenderer::draw_panel(UIDrawList& drawList,
                                  const EditorUI& editorUI,
                                  const UIPanel& panel) const {
    // Find the UIElement that represents this panel in the layout tree.
    const UIContext& context = editorUI.context();
    UIElement* panelElement = context.find_by_id(panel.id());
    if (!panelElement) {
        // Panel is not currently represented in the layout, skip drawing.
        return;
    }

    // Special case viewport panels for now.
    if (panel.kind() == PanelKind::Viewport) {
        const auto* viewport = dynamic_cast<const ViewportPanel*>(&panel);
        if (viewport) {
            draw_viewport_panel(drawList, editorUI, *viewport);
            return;
        }
    }

    // Default behaviour for all other panel types.
    draw_generic_panel(drawList, editorUI, panel);
}

void EditorUIRenderer::draw_viewport_panel(UIDrawList& drawList,
                                           const EditorUI& editorUI,
                                           const ViewportPanel& viewport) const {
    const UIContext& context = editorUI.context();
    UIElement* panelElement = context.find_by_id(viewport.id());
    if (!panelElement) {
        return;
    }

    const Rect& rect = panelElement->global_rect();

    // Background + title bar
    drawList.draw_panel_background(*panelElement);
    drawList.draw_panel_titlebar(*panelElement, viewport.title());

    // Viewport content area (below title bar)
    const float titleBarHeight = 26.0f;

    float contentX = rect.x;
    float contentY = rect.y + titleBarHeight;
    float contentW = rect.width;
    float contentH = rect.height - titleBarHeight;

    if (contentW <= 0.0f || contentH <= 0.0f) {
        return;
    }

    // Optional: darken the content background slightly
    drawList.draw_rect(
        contentX, contentY, contentW, contentH,
        0.05f, 0.05f, 0.05f, 1.0f
    );

    // If the viewport has a render texture, draw it.
    const std::uint64_t textureId = viewport.render_texture_id();
    if (textureId != 0 && contentW > 0.0f && contentH > 0.0f) {
        drawList.draw_image(
            contentX, contentY, contentW, contentH,
            textureId
        );
    }
}

void EditorUIRenderer::draw_generic_panel(UIDrawList& drawList,
                                          const EditorUI& editorUI,
                                          const UIPanel& panel) const {
    const UIContext& context = editorUI.context();
    UIElement* panelElement = context.find_by_id(panel.id());
    if (!panelElement) {
        return;
    }

    const Rect& rect = panelElement->global_rect();

    // Background and title bar using helper functions.
    drawList.draw_panel_background(*panelElement);
    drawList.draw_panel_titlebar(*panelElement, panel.title());

    // Simple content background below the title bar.
    const float titleBarHeight = 26.0f;

    float contentX = rect.x;
    float contentY = rect.y + titleBarHeight;
    float contentW = rect.width;
    float contentH = rect.height - titleBarHeight;

    if (contentW <= 0.0f || contentH <= 0.0f) {
        return;
    }

    drawList.draw_rect(
        contentX, contentY, contentW, contentH,
        0.10f, 0.10f, 0.10f, 1.0f
    );

    // Later you can specialise rendering for:
    //  - ConsolePanel: draw lines of text.
    //  - ResourceBrowserPanel: draw icons and file names.
    //  - StatisticsPanel: draw graphs / numbers.
}

} // namespace wave::editor::ui
