#pragma once

#include "../../math/mat4.hpp"
#include "../../render/vulkan/vk_frame_uniforms.hpp"

namespace wave::engine::render::camera {

using wave::engine::math::Vec3;
using wave::engine::math::Mat4;
using wave::engine::render::vulkan::GlobalFrameUBO;

// Simple perspective camera for the engine/editor.
class Camera {
public:
    Camera() = default;

    // Construct a camera with basic parameters.
    Camera(const Vec3& position,
           const Vec3& target,
           const Vec3& up,
           float fovYRadians,
           float aspect,
           float nearPlane,
           float farPlane)
        : m_position(position)
        , m_target(target)
        , m_up(up)
        , m_fovY(fovYRadians)
        , m_aspect(aspect)
        , m_near(nearPlane)
        , m_far(farPlane) {}

    const Vec3& position() const { return m_position; }
    const Vec3& target()   const { return m_target; }
    const Vec3& up()       const { return m_up; }

    void set_position(const Vec3& p) { m_position = p; }
    void set_target(const Vec3& t)   { m_target   = t; }
    void set_up(const Vec3& u)       { m_up       = u; }

    float fov_y() const { return m_fovY; }
    float aspect() const { return m_aspect; }
    float near_plane() const { return m_near; }
    float far_plane()  const { return m_far; }

    void set_fov_y(float radians)   { m_fovY  = radians; }
    void set_aspect(float aspect)   { m_aspect = aspect; }
    void set_near_plane(float n)    { m_near   = n; }
    void set_far_plane(float f)     { m_far    = f; }

    Mat4 view() const;
    Mat4 proj() const;
    Mat4 view_proj() const;

private:
    Vec3  m_position{0.0f, 0.0f, -5.0f};
    Vec3  m_target{0.0f, 0.0f, 0.0f};
    Vec3  m_up{0.0f, 1.0f, 0.0f};

    float m_fovY{60.0f * 3.1415926535f / 180.0f};
    float m_aspect{16.0f / 9.0f};
    float m_near{0.1f};
    float m_far{1000.0f};
};

// Helper to fill GlobalFrameUBO using a Camera.
void fill_global_frame_ubo(const Camera& cam,
                           float timeSeconds,
                           float deltaSeconds,
                           GlobalFrameUBO& outUBO);

} // namespace wave::engine::render::camera
