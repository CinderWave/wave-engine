#include "console_panel.hpp"

#include <algorithm>

namespace wave::editor::ui {

// -----------------------------------------------------------------------------
// Construction
// -----------------------------------------------------------------------------

ConsolePanel::ConsolePanel(std::string id, std::string title)
    : UIPanel(std::move(id), std::move(title), PanelKind::Console) {}

// -----------------------------------------------------------------------------
// Message API
// -----------------------------------------------------------------------------

void ConsolePanel::add_message(ConsoleMessage msg) {
    m_messages.push_back(std::move(msg));
    trim_if_needed();
}

void ConsolePanel::add_message(const std::string& text,
                               ConsoleSeverity severity,
                               std::uint64_t timestamp,
                               const std::string& category) {
    ConsoleMessage msg{};
    msg.text      = text;
    msg.severity  = severity;
    msg.timestamp = timestamp;
    msg.category  = category;

    add_message(std::move(msg));
}

void ConsolePanel::clear() {
    m_messages.clear();
}

// -----------------------------------------------------------------------------
// Filtering
// -----------------------------------------------------------------------------

void ConsolePanel::set_text_filter(const std::string& filter) {
    m_textFilter = filter;
}

bool ConsolePanel::passes_filter(const ConsoleMessage& msg) const {
    // Severity filter
    switch (msg.severity) {
    case ConsoleSeverity::Info:
        if (!m_showInfo) {
            return false;
        }
        break;
    case ConsoleSeverity::Warning:
        if (!m_showWarnings) {
            return false;
        }
        break;
    case ConsoleSeverity::Error:
        if (!m_showErrors) {
            return false;
        }
        break;
    }

    // Text filter (simple substring, case sensitive for now)
    if (!m_textFilter.empty()) {
        if (msg.text.find(m_textFilter) == std::string::npos &&
            msg.category.find(m_textFilter) == std::string::npos) {
            return false;
        }
    }

    return true;
}

// -----------------------------------------------------------------------------
// Capacity
// -----------------------------------------------------------------------------

void ConsolePanel::set_max_messages(std::size_t maxMessages) {
    m_maxMessages = maxMessages;
    trim_if_needed();
}

void ConsolePanel::trim_if_needed() {
    if (m_maxMessages == 0) {
        return;
    }

    while (m_messages.size() > m_maxMessages) {
        m_messages.pop_front();
    }
}

} // namespace wave::editor::ui
