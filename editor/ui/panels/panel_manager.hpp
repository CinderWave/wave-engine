#pragma once

#include "ui_panel.hpp"

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

namespace wave::editor::ui {

// High level docking target for a panel.
// Actual pixel layout will come later; this is intent only.
enum class DockSlot {
    Left,
    Right,
    Top,
    Bottom,
    Center,
    Floating
};

struct PanelEntry {
    std::unique_ptr<UIPanel> panel;
    DockSlot dockSlot{DockSlot::Center};
    bool visible{true};
};

class PanelManager final {
public:
    PanelManager() = default;
    ~PanelManager() = default;

    PanelManager(const PanelManager&) = delete;
    PanelManager& operator=(const PanelManager&) = delete;

    PanelManager(PanelManager&&) noexcept = default;
    PanelManager& operator=(PanelManager&&) noexcept = default;

    // Register a panel. Ownership is transferred to the manager.
    // If a panel with the same id already exists, it will be replaced.
    UIPanel& register_panel(std::unique_ptr<UIPanel> panel, DockSlot dockSlot);

    // Lookup by id. Returns nullptr if not found.
    UIPanel*       find_panel(const std::string& id);
    const UIPanel* find_panel(const std::string& id) const;

    // Visibility control -------------------------------------------------------

    void show_panel(const std::string& id);
    void hide_panel(const std::string& id);
    void toggle_panel(const std::string& id);

    bool is_visible(const std::string& id) const;

    // Docking intent -----------------------------------------------------------

    void set_dock_slot(const std::string& id, DockSlot slot);
    DockSlot dock_slot(const std::string& id) const;

    // Access to all entries (for layout / rendering passes)
    const std::vector<PanelEntry>& panels() const { return m_panels; }

private:
    PanelEntry*       find_entry(const std::string& id);
    const PanelEntry* find_entry(const std::string& id) const;

private:
    std::vector<PanelEntry> m_panels;
    std::unordered_map<std::string, std::size_t> m_indexById;
};

} // namespace wave::editor::ui
