#include "ui_context.hpp"

namespace wave::engine::ui {

// -----------------------------------------------------------------------------
// Root management
// -----------------------------------------------------------------------------

void UIContext::set_root(std::unique_ptr<UIElement> root) {
    m_root = std::move(root);
}

UIElement* UIContext::root() {
    return m_root.get();
}

const UIElement* UIContext::root() const {
    return m_root.get();
}

// -----------------------------------------------------------------------------
// Layout
// -----------------------------------------------------------------------------

void UIContext::update_layout(const Rect& screenRect) {
    if (!m_root) {
        return;
    }

    m_root->update_layout(screenRect);
}

// -----------------------------------------------------------------------------
// Find by id
// -----------------------------------------------------------------------------

UIElement* UIContext::find_by_id(const std::string& id) {
    return find_by_id_internal(m_root.get(), id);
}

const UIElement* UIContext::find_by_id(const std::string& id) const {
    return find_by_id_internal(m_root.get(), id);
}

UIElement* UIContext::find_by_id_internal(UIElement* node, const std::string& id) const {
    if (!node) {
        return nullptr;
    }

    if (node->id() == id) {
        return node;
    }

    auto& children = node->children();
    for (auto& childPtr : children) {
        if (!childPtr) {
            continue;
        }
        if (UIElement* found = find_by_id_internal(childPtr.get(), id)) {
            return found;
        }
    }

    return nullptr;
}

// -----------------------------------------------------------------------------
// Hit testing
// -----------------------------------------------------------------------------

UIElement* UIContext::hit_test(float x, float y) {
    return hit_test_internal(m_root.get(), x, y);
}

const UIElement* UIContext::hit_test(float x, float y) const {
    return hit_test_internal(m_root.get(), x, y);
}

UIElement* UIContext::hit_test_internal(UIElement* node, float x, float y) const {
    if (!node || !node->visible()) {
        return nullptr;
    }

    const Rect& rect = node->global_rect();
    const bool inside =
        x >= rect.x &&
        y >= rect.y &&
        x <= rect.x + rect.width &&
        y <= rect.y + rect.height;

    if (!inside) {
        return nullptr;
    }

    // Check children first so deepest element wins.
    UIElement* best = nullptr;

    auto& children = node->children();
    for (auto& childPtr : children) {
        if (!childPtr) {
            continue;
        }

        if (UIElement* hit = hit_test_internal(childPtr.get(), x, y)) {
            best = hit;
        }
    }

    // If no child claims it, this node is the hit.
    return best ? best : node;
}

} // namespace wave::engine::ui
