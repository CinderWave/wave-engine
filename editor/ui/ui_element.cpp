#include "ui_element.hpp"

namespace wave::engine::ui {

// -----------------------------------------------------------------------------
// Construction
// -----------------------------------------------------------------------------

UIElement::UIElement(std::string id)
    : m_id(std::move(id)) {}

// -----------------------------------------------------------------------------
// Hierarchy
// -----------------------------------------------------------------------------

UIElement& UIElement::add_child(std::unique_ptr<UIElement> child) {
    if (!child) {
        return *this;
    }

    child->m_parent = this;
    m_children.push_back(std::move(child));
    m_layoutDirty = true;
    return *m_children.back();
}

// -----------------------------------------------------------------------------
// Layout
// -----------------------------------------------------------------------------

void UIElement::update_layout(const Rect& parentGlobalRect) {
    if (!m_visible) {
        return;
    }

    update_self_rect(parentGlobalRect);

    // Layout children relative to this element
    layout_children();

    // Propagate layout to children
    for (auto& child : m_children) {
        child->update_layout(m_globalRect);
    }

    m_layoutDirty = false;
}

void UIElement::update_self_rect(const Rect& parentGlobalRect) {
    if (!m_layoutDirty && m_globalRect.width != 0.0f && m_globalRect.height != 0.0f) {
        return;
    }

    Rect rect = m_localRect;

    switch (m_anchor) {
    case Anchor::TopLeft:
        rect.x += parentGlobalRect.x;
        rect.y += parentGlobalRect.y;
        break;
    case Anchor::TopRight:
        rect.x = parentGlobalRect.x + parentGlobalRect.width - rect.width - rect.x;
        rect.y += parentGlobalRect.y;
        break;
    case Anchor::BottomLeft:
        rect.x += parentGlobalRect.x;
        rect.y = parentGlobalRect.y + parentGlobalRect.height - rect.height - rect.y;
        break;
    case Anchor::BottomRight:
        rect.x = parentGlobalRect.x + parentGlobalRect.width - rect.width - rect.x;
        rect.y = parentGlobalRect.y + parentGlobalRect.height - rect.height - rect.y;
        break;
    case Anchor::Center:
        rect.x = parentGlobalRect.x + (parentGlobalRect.width  - rect.width)  * 0.5f + rect.x;
        rect.y = parentGlobalRect.y + (parentGlobalRect.height - rect.height) * 0.5f + rect.y;
        break;
    }

    m_globalRect = rect;
}

void UIElement::layout_children() {
    if (m_children.empty()) {
        return;
    }

    if (m_layoutMode == LayoutMode::Absolute) {
        return;
    }

    float cursorX = m_globalRect.x + m_padding.x;
    float cursorY = m_globalRect.y + m_padding.y;

    for (auto& child : m_children) {
        Rect childLocal = child->m_localRect;

        if (m_layoutMode == LayoutMode::Vertical) {
            childLocal.x = m_padding.x;
            childLocal.y = cursorY - m_globalRect.y;

            cursorY += childLocal.height + m_padding.y;
        } else if (m_layoutMode == LayoutMode::Horizontal) {
            childLocal.x = cursorX - m_globalRect.x;
            childLocal.y = m_padding.y;

            cursorX += childLocal.width + m_padding.x;
        }

        child->m_localRect = childLocal;
        child->m_layoutDirty = true;
    }
}

} // namespace wave::engine::ui
