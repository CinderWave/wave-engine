#pragma once

#include "../ui_element.hpp"

#include <string>

namespace wave::editor::ui {

enum class PanelKind {
    Generic,
    Console,
    Viewport,
    ResourceBrowser,
    Statistics
};

class UIPanel : public UIElement {
public:
    UIPanel() = default;
    UIPanel(std::string id, std::string title, PanelKind kind = PanelKind::Generic);

    UIPanel(const UIPanel&) = delete;
    UIPanel& operator=(const UIPanel&) = delete;

    UIPanel(UIPanel&&) noexcept = default;
    UIPanel& operator=(UIPanel&&) noexcept = default;

    ~UIPanel() override = default;

    // Panel meta info ----------------------------------------------------------

    void set_title(const std::string& title) { m_title = title; }
    const std::string& title() const { return m_title; }

    void set_kind(PanelKind kind) { m_kind = kind; }
    PanelKind kind() const { return m_kind; }

    void set_closable(bool closable) { m_closable = closable; }
    bool closable() const { return m_closable; }

    void set_movable(bool movable) { m_movable = movable; }
    bool movable() const { return m_movable; }

    void set_focused(bool focused) { m_focused = focused; }
    bool focused() const { return m_focused; }

    void set_hovered(bool hovered) { m_hovered = hovered; }
    bool hovered() const { return m_hovered; }

private:
    std::string m_title;
    PanelKind   m_kind{PanelKind::Generic};

    bool m_closable{true};
    bool m_movable{true};

    bool m_focused{false};
    bool m_hovered{false};
};

} // namespace wave::editor::ui
