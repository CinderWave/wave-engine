#include "docking_layout.hpp"

namespace wave::editor::ui {

// -----------------------------------------------------------------------------
// Public API
// -----------------------------------------------------------------------------

void DockingLayout::rebuild(UIContext& context, PanelManager& panels) {
    UIElement* root = context.root();
    if (!root) {
        return;
    }

    // Expected root children (created by EditorUI):
    //   dock_left
    //   dock_center
    //   dock_right

    UIElement* dockLeft   = get_dock(context, "dock_left");
    UIElement* dockCenter = get_dock(context, "dock_center");
    UIElement* dockRight  = get_dock(context, "dock_right");

    if (!dockLeft || !dockCenter || !dockRight) {
        // EditorUI not initialized yet.
        return;
    }

    // Clear all dock children (panels will be re-added)
    dockLeft->children().clear();
    dockCenter->children().clear();
    dockRight->children().clear();

    // Walk registered panels and place them
    for (const PanelEntry& entry : panels.panels()) {
        UIPanel* p = entry.panel.get();
        if (!p || !entry.visible) {
            continue;
        }

        switch (entry.dockSlot) {
        case DockSlot::Left:
            attach_to_dock(dockLeft, p);
            break;

        case DockSlot::Center:
            attach_to_dock(dockCenter, p);
            break;

        case DockSlot::Right:
            attach_to_dock(dockRight, p);
            break;

        // Reserved for future
        case DockSlot::Top:
        case DockSlot::Bottom:
        case DockSlot::Floating:
            attach_to_dock(dockCenter, p);
            break;
        }
    }

    // After modifying children, mark layout dirty for root
    context.update_layout(context.root()->global_rect());
}

// -----------------------------------------------------------------------------
// Internal helpers
// -----------------------------------------------------------------------------

UIElement* DockingLayout::get_dock(UIContext& context, const std::string& dockId) {
    return context.find_by_id(dockId);
}

void DockingLayout::attach_to_dock(UIElement* dockNode, UIPanel* panel) {
    if (!dockNode || !panel) {
        return;
    }

    // Each UIPanel is a UIElement.
    // Clone-free approach: panel is owned by PanelManager, but dockNode
    // must reference it in the layout tree, so we wrap it in a forwarding element.

    auto wrapper = std::make_unique<UIElement>(panel->id() + "_layout_wrapper");

    // Use panel's preferred size as placeholder
    Rect r{};
    r.x = 0.0f;
    r.y = 0.0f;
    r.width  = 300.0f;   // placeholder: real renderer will provide final size
    r.height = 200.0f;
    wrapper->set_local_rect(r);

    wrapper->set_layout_mode(LayoutMode::Vertical);

    // The panel itself becomes a child of the wrapper
    auto panelSlot = std::make_unique<UIElement>(panel->id());
    {
        Rect pr{};
        pr.x = 0.0f;
        pr.y = 0.0f;
        pr.width  = r.width;
        pr.height = r.height;
        panelSlot->set_local_rect(pr);
        panelSlot->set_anchor(Anchor::TopLeft);
    }

    wrapper->add_child(std::move(panelSlot));

    dockNode->add_child(std::move(wrapper));
}

} // namespace wave::editor::ui
