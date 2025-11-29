#pragma once

#include <vector>
#include <cstdint>

namespace wave::engine::render {

struct Vertex {
    float position[3]{};
    float normal[3]{};
    float uv[2]{};
};

class Mesh final {
public:
    Mesh() = default;

    Mesh(std::vector<Vertex> vertices, std::vector<std::uint32_t> indices);

    // Non copyable for now (can be added later if needed)
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    // Movable
    Mesh(Mesh&& other) noexcept;
    Mesh& operator=(Mesh&& other) noexcept;

    ~Mesh() = default;

    const std::vector<Vertex>&       vertices() const { return m_vertices; }
    const std::vector<std::uint32_t>& indices() const { return m_indices; }

    std::size_t vertex_count() const { return m_vertices.size(); }
    std::size_t index_count()  const { return m_indices.size();  }

    bool empty() const { return m_vertices.empty() || m_indices.empty(); }

    // Simple helpers for adding geometry
    void add_vertex(const Vertex& v);
    void add_index(std::uint32_t i);

    void clear();

private:
    std::vector<Vertex>        m_vertices;
    std::vector<std::uint32_t> m_indices;
};

} // namespace wave::engine::render
