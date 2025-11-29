#include "ui_panel.hpp"

namespace wave::editor::ui {

UIPanel::UIPanel(std::string id, std::string title, PanelKind kind)
    : UIElement(std::move(id))
    , m_title(std::move(title))
    , m_kind(kind) {}

} // namespace wave::editor::ui
