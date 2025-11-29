#pragma once

#include "../ui_panel.hpp"

#include <string>
#include <vector>
#include <cstdint>

namespace wave::editor::ui {

struct StatSample {
    float value{0.0f};
    std::uint64_t frame{0};
};

struct StatSeries {
    std::string name;
    std::string category;     // e.g. "Frame", "Render", "CPU", etc.
    std::string unit;         // e.g. "ms", "FPS", "MB"

    std::vector<StatSample> samples;
    std::size_t maxSamples{256};

    // Optional display flags
    bool visible{true};
};

class StatisticsPanel final : public UIPanel {
public:
    explicit StatisticsPanel(std::string id = "statistics",
                             std::string title = "Statistics");

    StatisticsPanel(const StatisticsPanel&) = delete;
    StatisticsPanel& operator=(const StatisticsPanel&) = delete;

    StatisticsPanel(StatisticsPanel&&) noexcept = default;
    StatisticsPanel& operator=(StatisticsPanel&&) noexcept = default;

    ~StatisticsPanel() override = default;

    // -------------------------------------------------------------------------
    // Series management
    // -------------------------------------------------------------------------

    // Ensure a series exists with the given name; returns its index.
    std::size_t ensure_series(const std::string& name,
                              const std::string& category = {},
                              const std::string& unit = {});

    // Returns nullptr if not found.
    StatSeries*       find_series(const std::string& name);
    const StatSeries* find_series(const std::string& name) const;

    // Append a sample to a series, creating it if needed.
    void add_sample(const std::string& seriesName,
                    float value,
                    std::uint64_t frame,
                    const std::string& category = {},
                    const std::string& unit = {});

    // Set maximum sample history length for a given series (or all).
    void set_series_max_samples(const std::string& name, std::size_t maxSamples);
    void set_all_series_max_samples(std::size_t maxSamples);

    // Access to all series for rendering.
    const std::vector<StatSeries>& series() const { return m_series; }

    // -------------------------------------------------------------------------
    // Visibility and filtering
    // -------------------------------------------------------------------------

    void set_series_visible(const std::string& name, bool visible);
    bool is_series_visible(const std::string& name) const;

    void set_category_filter(const std::string& category);
    const std::string& category_filter() const { return m_categoryFilter; }

    bool passes_filter(const StatSeries& s) const;

private:
    void trim_series(StatSeries& s);

private:
    std::vector<StatSeries> m_series;
    std::string m_categoryFilter;
};

} // namespace wave::editor::ui
