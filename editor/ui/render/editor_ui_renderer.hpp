#pragma once

#include "../editor_ui.hpp"
#include "ui_draw_list.hpp"

namespace wave::editor::ui {

// Converts the current EditorUI state (panels + layout)
// into a list of draw commands consumed by the renderer.
class EditorUIRenderer final {
public:
    EditorUIRenderer() = default;
    ~EditorUIRenderer() = default;

    EditorUIRenderer(const EditorUIRenderer&) = delete;
    EditorUIRenderer& operator=(const EditorUIRenderer&) = delete;

    EditorUIRenderer(EditorUIRenderer&&) noexcept = default;
    EditorUIRenderer& operator=(EditorUIRenderer&&) noexcept = default;

    // Build draw commands for the entire editor UI.
    // Call once per frame from the editor render loop.
    void build_draw_list(const EditorUI& editorUI, UIDrawList& drawList);

private:
    void draw_panel(UIDrawList& drawList, const EditorUI& editorUI, const UIPanel& panel) const;

    void draw_viewport_panel(UIDrawList& drawList,
                             const EditorUI& editorUI,
                             const class ViewportPanel& viewport) const;

    void draw_generic_panel(UIDrawList& drawList,
                            const EditorUI& editorUI,
                            const UIPanel& panel) const;
};

} // namespace wave::editor::ui
