#include "statistics_panel.hpp"

#include <algorithm>

namespace wave::editor::ui {

// -----------------------------------------------------------------------------
// Construction
// -----------------------------------------------------------------------------

StatisticsPanel::StatisticsPanel(std::string id, std::string title)
    : UIPanel(std::move(id), std::move(title), PanelKind::Statistics) {}

// -----------------------------------------------------------------------------
// Series management
// -----------------------------------------------------------------------------

std::size_t StatisticsPanel::ensure_series(
    const std::string& name,
    const std::string& category,
    const std::string& unit
) {
    auto it = std::find_if(
        m_series.begin(),
        m_series.end(),
        [&name](const StatSeries& s) { return s.name == name; }
    );

    if (it != m_series.end()) {
        return static_cast<std::size_t>(std::distance(m_series.begin(), it));
    }

    StatSeries s{};
    s.name     = name;
    s.category = category;
    s.unit     = unit;
    s.maxSamples = 256;
    s.visible  = true;

    m_series.push_back(std::move(s));
    return m_series.size() - 1;
}

StatSeries* StatisticsPanel::find_series(const std::string& name) {
    auto it = std::find_if(
        m_series.begin(),
        m_series.end(),
        [&name](const StatSeries& s) { return s.name == name; }
    );
    if (it == m_series.end()) {
        return nullptr;
    }
    return &(*it);
}

const StatSeries* StatisticsPanel::find_series(const std::string& name) const {
    auto it = std::find_if(
        m_series.begin(),
        m_series.end(),
        [&name](const StatSeries& s) { return s.name == name; }
    );
    if (it == m_series.end()) {
        return nullptr;
    }
    return &(*it);
}

void StatisticsPanel::add_sample(
    const std::string& seriesName,
    float value,
    std::uint64_t frame,
    const std::string& category,
    const std::string& unit
) {
    const std::size_t index = ensure_series(seriesName, category, unit);
    StatSeries& s = m_series[index];

    StatSample sample{};
    sample.value = value;
    sample.frame = frame;

    s.samples.push_back(sample);
    trim_series(s);
}

void StatisticsPanel::set_series_max_samples(const std::string& name, std::size_t maxSamples) {
    if (StatSeries* s = find_series(name)) {
        s->maxSamples = maxSamples;
        trim_series(*s);
    }
}

void StatisticsPanel::set_all_series_max_samples(std::size_t maxSamples) {
    for (auto& s : m_series) {
        s.maxSamples = maxSamples;
        trim_series(s);
    }
}

// -----------------------------------------------------------------------------
// Visibility / filtering
// -----------------------------------------------------------------------------

void StatisticsPanel::set_series_visible(const std::string& name, bool visible) {
    if (StatSeries* s = find_series(name)) {
        s->visible = visible;
    }
}

bool StatisticsPanel::is_series_visible(const std::string& name) const {
    if (const StatSeries* s = find_series(name)) {
        return s->visible;
    }
    return false;
}

void StatisticsPanel::set_category_filter(const std::string& category) {
    m_categoryFilter = category;
}

bool StatisticsPanel::passes_filter(const StatSeries& s) const {
    if (!m_categoryFilter.empty() && s.category != m_categoryFilter) {
        return false;
    }
    return true;
}

// -----------------------------------------------------------------------------
// Internal helpers
// -----------------------------------------------------------------------------

void StatisticsPanel::trim_series(StatSeries& s) {
    if (s.maxSamples == 0) {
        return;
    }

    const std::size_t size = s.samples.size();
    if (size <= s.maxSamples) {
        return;
    }

    const std::size_t excess = size - s.maxSamples;
    s.samples.erase(s.samples.begin(), s.samples.begin() + static_cast<std::ptrdiff_t>(excess));
}

} // namespace wave::editor::ui
