#pragma once

#include "../ui_panel.hpp"

#include <string>
#include <deque>
#include <cstdint>

namespace wave::editor::ui {

enum class ConsoleSeverity {
    Info,
    Warning,
    Error
};

struct ConsoleMessage {
    std::string     text;
    ConsoleSeverity severity{ConsoleSeverity::Info};
    std::uint64_t   timestamp{0};       // optional, e.g. frame counter or ms
    std::string     category;           // optional: "Render", "Assets", etc.
};

class ConsolePanel final : public UIPanel {
public:
    explicit ConsolePanel(std::string id = "console",
                          std::string title = "Console");

    ConsolePanel(const ConsolePanel&) = delete;
    ConsolePanel& operator=(const ConsolePanel&) = delete;

    ConsolePanel(ConsolePanel&&) noexcept = default;
    ConsolePanel& operator=(ConsolePanel&&) noexcept = default;

    ~ConsolePanel() override = default;

    // Message API --------------------------------------------------------------

    void add_message(ConsoleMessage msg);
    void add_message(const std::string& text,
                     ConsoleSeverity severity = ConsoleSeverity::Info,
                     std::uint64_t timestamp = 0,
                     const std::string& category = {});

    void clear();

    const std::deque<ConsoleMessage>& messages() const { return m_messages; }

    // Filtering ----------------------------------------------------------------

    void set_show_info(bool show) { m_showInfo = show; }
    void set_show_warnings(bool show) { m_showWarnings = show; }
    void set_show_errors(bool show) { m_showErrors = show; }

    bool show_info() const { return m_showInfo; }
    bool show_warnings() const { return m_showWarnings; }
    bool show_errors() const { return m_showErrors; }

    // Optional text filter (simple substring match)
    void set_text_filter(const std::string& filter);
    const std::string& text_filter() const { return m_textFilter; }

    // Returns whether a message should be visible given current filters.
    bool passes_filter(const ConsoleMessage& msg) const;

    // Capacity -----------------------------------------------------------------

    void set_max_messages(std::size_t maxMessages);
    std::size_t max_messages() const { return m_maxMessages; }

private:
    void trim_if_needed();

private:
    std::deque<ConsoleMessage> m_messages;

    std::size_t m_maxMessages{1024};

    bool        m_showInfo{true};
    bool        m_showWarnings{true};
    bool        m_showErrors{true};

    std::string m_textFilter;
};

} // namespace wave::editor::ui
