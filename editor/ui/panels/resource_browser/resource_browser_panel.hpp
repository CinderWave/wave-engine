#pragma once

#include "../ui_panel.hpp"

#include <string>
#include <vector>
#include <cstdint>

namespace wave::editor::ui {

struct ResourceEntry {
    std::string name;
    std::string fullPath;   // engine-relative or absolute, up to caller
    bool        isDirectory{false};
};

class ResourceBrowserPanel final : public UIPanel {
public:
    explicit ResourceBrowserPanel(std::string id = "resource_browser",
                                  std::string title = "Resources");

    ResourceBrowserPanel(const ResourceBrowserPanel&) = delete;
    ResourceBrowserPanel& operator=(const ResourceBrowserPanel&) = delete;

    ResourceBrowserPanel(ResourceBrowserPanel&&) noexcept = default;
    ResourceBrowserPanel& operator=(ResourceBrowserPanel&&) noexcept = default;

    ~ResourceBrowserPanel() override = default;

    // -------------------------------------------------------------------------
    // Path management
    // -------------------------------------------------------------------------

    void set_root_path(const std::string& path);
    const std::string& root_path() const { return m_rootPath; }

    void set_current_path(const std::string& path);
    const std::string& current_path() const { return m_currentPath; }

    // Go to parent directory (if any); returns true if changed.
    bool navigate_up();

    // -------------------------------------------------------------------------
    // Entries
    // -------------------------------------------------------------------------

    void set_entries(std::vector<ResourceEntry> entries);
    const std::vector<ResourceEntry>& entries() const { return m_entries; }

    bool empty() const { return m_entries.empty(); }

    // -------------------------------------------------------------------------
    // Selection
    // -------------------------------------------------------------------------

    void clear_selection();

    // Select by index (e.g. coming from UI hit testing)
    void select_index(std::size_t index);
    bool is_selected_index(std::size_t index) const;

    // Select by path (helper for external systems)
    void select_path(const std::string& fullPath);
    bool is_selected_path(const std::string& fullPath) const;

    const ResourceEntry* selected_entry() const;

    // -------------------------------------------------------------------------
    // Simple filtering
    // -------------------------------------------------------------------------

    void set_extension_filter(const std::string& extension); // e.g. ".png"
    const std::string& extension_filter() const { return m_extensionFilter; }

    // Whether an entry should be visible given the current filter.
    bool passes_filter(const ResourceEntry& entry) const;

private:
    std::string m_rootPath;
    std::string m_currentPath;

    std::vector<ResourceEntry> m_entries;

    // Selection is stored as fullPath; index can change when list is refreshed.
    std::string m_selectedPath;

    std::string m_extensionFilter;
};

} // namespace wave::editor::ui
