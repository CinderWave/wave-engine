#include "panel_manager.hpp"

namespace wave::editor::ui {

// -----------------------------------------------------------------------------
// Registration
// -----------------------------------------------------------------------------

UIPanel& PanelManager::register_panel(std::unique_ptr<UIPanel> panel, DockSlot dockSlot) {
    if (!panel) {
        // This should not happen in normal editor code.
        // Return a dummy reference by creating a placeholder generic panel.
        auto fallback = std::make_unique<UIPanel>("__invalid_panel__", "Invalid");
        m_panels.push_back(PanelEntry{ std::move(fallback), DockSlot::Center, false });
        m_indexById[m_panels.back().panel->id()] = m_panels.size() - 1;
        return *m_panels.back().panel;
    }

    const std::string id = panel->id();

    // Replace existing entry if id already registered
    if (auto it = m_indexById.find(id); it != m_indexById.end()) {
        std::size_t index = it->second;
        m_panels[index].panel    = std::move(panel);
        m_panels[index].dockSlot = dockSlot;
        m_panels[index].visible  = true;
        return *m_panels[index].panel;
    }

    // New panel
    PanelEntry entry{};
    entry.panel    = std::move(panel);
    entry.dockSlot = dockSlot;
    entry.visible  = true;

    m_panels.push_back(std::move(entry));
    const std::size_t newIndex = m_panels.size() - 1;
    m_indexById[m_panels[newIndex].panel->id()] = newIndex;

    return *m_panels[newIndex].panel;
}

// -----------------------------------------------------------------------------
// Lookup
// -----------------------------------------------------------------------------

UIPanel* PanelManager::find_panel(const std::string& id) {
    if (auto* entry = find_entry(id)) {
        return entry->panel.get();
    }
    return nullptr;
}

const UIPanel* PanelManager::find_panel(const std::string& id) const {
    if (auto* entry = find_entry(id)) {
        return entry->panel.get();
    }
    return nullptr;
}

PanelEntry* PanelManager::find_entry(const std::string& id) {
    auto it = m_indexById.find(id);
    if (it == m_indexById.end()) {
        return nullptr;
    }

    const std::size_t index = it->second;
    if (index >= m_panels.size()) {
        return nullptr;
    }

    return &m_panels[index];
}

const PanelEntry* PanelManager::find_entry(const std::string& id) const {
    auto it = m_indexById.find(id);
    if (it == m_indexById.end()) {
        return nullptr;
    }

    const std::size_t index = it->second;
    if (index >= m_panels.size()) {
        return nullptr;
    }

    return &m_panels[index];
}

// -----------------------------------------------------------------------------
// Visibility
// -----------------------------------------------------------------------------

void PanelManager::show_panel(const std::string& id) {
    if (auto* entry = find_entry(id)) {
        entry->visible = true;
    }
}

void PanelManager::hide_panel(const std::string& id) {
    if (auto* entry = find_entry(id)) {
        entry->visible = false;
    }
}

void PanelManager::toggle_panel(const std::string& id) {
    if (auto* entry = find_entry(id)) {
        entry->visible = !entry->visible;
    }
}

bool PanelManager::is_visible(const std::string& id) const {
    if (auto* entry = find_entry(id)) {
        return entry->visible;
    }
    return false;
}

// -----------------------------------------------------------------------------
// Docking intent
// -----------------------------------------------------------------------------

void PanelManager::set_dock_slot(const std::string& id, DockSlot slot) {
    if (auto* entry = find_entry(id)) {
        entry->dockSlot = slot;
    }
}

DockSlot PanelManager::dock_slot(const std::string& id) const {
    if (auto* entry = find_entry(id)) {
        return entry->dockSlot;
    }
    return DockSlot::Center;
}

} // namespace wave::editor::ui
