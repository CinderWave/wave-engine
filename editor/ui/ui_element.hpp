#pragma once

#include <vector>
#include <string>
#include <cstdint>
#include <memory>

namespace wave::engine::ui {

struct Vec2 {
    float x{0.0f};
    float y{0.0f};
};

struct Rect {
    float x{0.0f};
    float y{0.0f};
    float width{0.0f};
    float height{0.0f};
};

enum class Anchor {
    TopLeft,
    TopRight,
    BottomLeft,
    BottomRight,
    Center
};

enum class LayoutMode {
    Absolute,   // position is interpreted in parent space
    Vertical,   // children stacked vertically
    Horizontal  // children stacked horizontally
};

class UIElement {
public:
    UIElement() = default;
    explicit UIElement(std::string id);

    UIElement(const UIElement&) = delete;
    UIElement& operator=(const UIElement&) = delete;

    UIElement(UIElement&&) noexcept = default;
    UIElement& operator=(UIElement&&) noexcept = default;

    ~UIElement() = default;

    // Identity / hierarchy -----------------------------------------------------

    const std::string& id() const { return m_id; }

    UIElement* parent() const { return m_parent; }

    const std::vector<std::unique_ptr<UIElement>>& children() const {
        return m_children;
    }

    UIElement& add_child(std::unique_ptr<UIElement> child);

    // Layout -------------------------------------------------------------------

    void set_local_rect(const Rect& rect) { m_localRect = rect; m_layoutDirty = true; }
    const Rect& local_rect() const { return m_localRect; }

    const Rect& global_rect() const { return m_globalRect; }

    void set_anchor(Anchor anchor) { m_anchor = anchor; m_layoutDirty = true; }
    Anchor anchor() const { return m_anchor; }

    void set_layout_mode(LayoutMode mode) { m_layoutMode = mode; m_layoutDirty = true; }
    LayoutMode layout_mode() const { return m_layoutMode; }

    void set_padding(const Vec2& padding) { m_padding = padding; m_layoutDirty = true; }
    const Vec2& padding() const { return m_padding; }

    void set_visible(bool visible) { m_visible = visible; }
    bool visible() const { return m_visible; }

    // Recalculate global rect and optionally layout children.
    // parentGlobalRect is used for the root if needed.
    void update_layout(const Rect& parentGlobalRect);

private:
    void update_self_rect(const Rect& parentGlobalRect);
    void layout_children();

private:
    std::string m_id;

    UIElement* m_parent{nullptr};
    std::vector<std::unique_ptr<UIElement>> m_children;

    Rect   m_localRect{};
    Rect   m_globalRect{};
    Anchor m_anchor{Anchor::TopLeft};
    LayoutMode m_layoutMode{LayoutMode::Absolute};
    Vec2   m_padding{};

    bool   m_visible{true};
    bool   m_layoutDirty{true};
};

} // namespace wave::engine::ui
