#include "resource_browser_panel.hpp"

#include <algorithm>

namespace wave::editor::ui {

// -----------------------------------------------------------------------------
// Construction
// -----------------------------------------------------------------------------

ResourceBrowserPanel::ResourceBrowserPanel(std::string id, std::string title)
    : UIPanel(std::move(id), std::move(title), PanelKind::ResourceBrowser) {}

// -----------------------------------------------------------------------------
// Paths
// -----------------------------------------------------------------------------

void ResourceBrowserPanel::set_root_path(const std::string& path) {
    m_rootPath = path;
    // If current path is empty or not under root, clamp to root
    if (m_currentPath.empty()) {
        m_currentPath = m_rootPath;
    }
}

void ResourceBrowserPanel::set_current_path(const std::string& path) {
    m_currentPath = path;
}

bool ResourceBrowserPanel::navigate_up() {
    if (m_currentPath.empty() || m_currentPath == m_rootPath) {
        return false;
    }

    // Very simple path trimming; real implementation may use a path library.
    auto pos = m_currentPath.find_last_of("/\\");
    if (pos == std::string::npos) {
        m_currentPath = m_rootPath;
        return true;
    }

    std::string parent = m_currentPath.substr(0, pos);
    if (parent.empty()) {
        parent = m_rootPath;
    }

    if (parent == m_currentPath) {
        return false;
    }

    m_currentPath = parent;
    return true;
}

// -----------------------------------------------------------------------------
// Entries
// -----------------------------------------------------------------------------

void ResourceBrowserPanel::set_entries(std::vector<ResourceEntry> entries) {
    m_entries = std::move(entries);

    // If selection path no longer exists in the list, clear it.
    if (!m_selectedPath.empty()) {
        auto it = std::find_if(
            m_entries.begin(),
            m_entries.end(),
            [this](const ResourceEntry& e) {
                return e.fullPath == m_selectedPath;
            }
        );

        if (it == m_entries.end()) {
            m_selectedPath.clear();
        }
    }
}

// -----------------------------------------------------------------------------
// Selection
// -----------------------------------------------------------------------------

void ResourceBrowserPanel::clear_selection() {
    m_selectedPath.clear();
}

void ResourceBrowserPanel::select_index(std::size_t index) {
    if (index >= m_entries.size()) {
        m_selectedPath.clear();
        return;
    }

    m_selectedPath = m_entries[index].fullPath;
}

bool ResourceBrowserPanel::is_selected_index(std::size_t index) const {
    if (index >= m_entries.size()) {
        return false;
    }

    return m_entries[index].fullPath == m_selectedPath;
}

void ResourceBrowserPanel::select_path(const std::string& fullPath) {
    // Only select if it exists in the current entries
    auto it = std::find_if(
        m_entries.begin(),
        m_entries.end(),
        [&fullPath](const ResourceEntry& e) {
            return e.fullPath == fullPath;
        }
    );

    if (it != m_entries.end()) {
        m_selectedPath = fullPath;
    } else {
        m_selectedPath.clear();
    }
}

bool ResourceBrowserPanel::is_selected_path(const std::string& fullPath) const {
    return m_selectedPath == fullPath;
}

const ResourceEntry* ResourceBrowserPanel::selected_entry() const {
    if (m_selectedPath.empty()) {
        return nullptr;
    }

    auto it = std::find_if(
        m_entries.begin(),
        m_entries.end(),
        [this](const ResourceEntry& e) {
            return e.fullPath == m_selectedPath;
        }
    );

    if (it == m_entries.end()) {
        return nullptr;
    }

    return &(*it);
}

// -----------------------------------------------------------------------------
// Filtering
// -----------------------------------------------------------------------------

void ResourceBrowserPanel::set_extension_filter(const std::string& extension) {
    m_extensionFilter = extension;
}

bool ResourceBrowserPanel::passes_filter(const ResourceEntry& entry) const {
    // Directories always pass filter so navigation isn't broken.
    if (entry.isDirectory) {
        return true;
    }

    if (m_extensionFilter.empty()) {
        return true;
    }

    // Simple suffix match; case sensitive for now.
    if (entry.fullPath.size() < m_extensionFilter.size()) {
        return false;
    }

    const std::size_t offset = entry.fullPath.size() - m_extensionFilter.size();
    return entry.fullPath.compare(offset, m_extensionFilter.size(), m_extensionFilter) == 0;
}

} // namespace wave::editor::ui
