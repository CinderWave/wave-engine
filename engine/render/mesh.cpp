#include "mesh.hpp"

#include <utility>

namespace wave::engine::render {

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<std::uint32_t> indices)
    : m_vertices(std::move(vertices))
    , m_indices(std::move(indices)) {}

Mesh::Mesh(Mesh&& other) noexcept
    : m_vertices(std::move(other.m_vertices))
    , m_indices(std::move(other.m_indices)) {}

Mesh& Mesh::operator=(Mesh&& other) noexcept {
    if (this != &other) {
        m_vertices = std::move(other.m_vertices);
        m_indices  = std::move(other.m_indices);
    }
    return *this;
}

void Mesh::add_vertex(const Vertex& v) {
    m_vertices.push_back(v);
}

void Mesh::add_index(std::uint32_t i) {
    m_indices.push_back(i);
}

void Mesh::clear() {
    m_vertices.clear();
    m_indices.clear();
}

} // namespace wave::engine::render
