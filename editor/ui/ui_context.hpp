#pragma once

#include "ui_element.hpp"

#include <memory>
#include <string>
#include <functional>

namespace wave::engine::ui {

// Central owner of the UI tree for a given viewport / window.
// Handles:
//  - root element ownership
//  - layout updates
//  - basic hit testing
//  - lookup by id
class UIContext final {
public:
    UIContext() = default;
    ~UIContext() = default;

    UIContext(const UIContext&) = delete;
    UIContext& operator=(const UIContext&) = delete;

    UIContext(UIContext&&) noexcept = default;
    UIContext& operator=(UIContext&&) noexcept = default;

    // Set the root element of this UI tree.
    // Passing nullptr clears the tree.
    void set_root(std::unique_ptr<UIElement> root);

    UIElement* root();
    const UIElement* root() const;

    // Update layout for the entire tree.
    // screenRect represents the full area available for UI (e.g. window size).
    void update_layout(const Rect& screenRect);

    // Shallow utility queries ----------------------------------------------

    // Find first element in the tree with a matching id (depth first).
    UIElement* find_by_id(const std::string& id);
    const UIElement* find_by_id(const std::string& id) const;

    // Hit test: return the deepest visible element whose global rect contains (x, y).
    UIElement* hit_test(float x, float y);
    const UIElement* hit_test(float x, float y) const;

private:
    UIElement* find_by_id_internal(UIElement* node, const std::string& id) const;
    UIElement* hit_test_internal(UIElement* node, float x, float y) const;

private:
    std::unique_ptr<UIElement> m_root;
};

} // namespace wave::engine::ui
