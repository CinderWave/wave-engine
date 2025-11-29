#pragma once

#include "../ui_context.hpp"
#include "../panels/panel_manager.hpp"
#include "../ui_element.hpp"
#include "../panels/ui_panel.hpp"

#include <string>
#include <vector>

namespace wave::editor::ui {

// Responsible for placing UIPanels into the UIContext tree
// based on their DockSlot (Left, Center, Right, etc).
// This does not do drag-drop docking yet; it only constructs
// a deterministic layout driven by PanelManager.
class DockingLayout final {
public:
    DockingLayout() = default;
    ~DockingLayout() = default;

    DockingLayout(const DockingLayout&) = delete;
    DockingLayout& operator=(const DockingLayout&) = delete;

    DockingLayout(DockingLayout&&) noexcept = default;
    DockingLayout& operator=(DockingLayout&&) noexcept = default;

    // Rebuilds panel nodes under the UIContext root.
    // This should be called after:
    //   - editor startup
    //   - panel visibility changes
    //   - layout mode changes
    //   - panel registration
    void rebuild(UIContext& context, PanelManager& panels);

private:
    // Helpers for placing panels into dock regions
    UIElement* get_dock(UIContext& context, const std::string& dockId);

    void attach_to_dock(UIElement* dockNode, UIPanel* panel);

private:
    // No persistent state; layout is regenerated each call.
};

} // namespace wave::editor::ui
