#include "camera.hpp"

namespace wave::engine::render::camera {

using wave::engine::math::Mat4;

// -----------------------------------------------------------------------------
// Camera matrices
// -----------------------------------------------------------------------------

Mat4 Camera::view() const {
    return Mat4::look_at(m_position, m_target, m_up);
}

Mat4 Camera::proj() const {
    return Mat4::perspective(m_fovY, m_aspect, m_near, m_far);
}

Mat4 Camera::view_proj() const {
    return Mat4::multiply(proj(), view());
}

// -----------------------------------------------------------------------------
// UBO fill helper
// -----------------------------------------------------------------------------

void fill_global_frame_ubo(const Camera& cam,
                           float timeSeconds,
                           float deltaSeconds,
                           GlobalFrameUBO& outUBO) {
    Mat4 v  = cam.view();
    Mat4 p  = cam.proj();
    Mat4 vp = Mat4::multiply(p, v);

    // Copy raw floats into the UBO arrays
    for (int i = 0; i < 16; ++i) {
        outUBO.view[i]     = v.m[i];
        outUBO.proj[i]     = p.m[i];
        outUBO.viewProj[i] = vp.m[i];
    }

    outUBO.time      = timeSeconds;
    outUBO.deltaTime = deltaSeconds;
    outUBO.padding0[0] = 0.0f;
    outUBO.padding0[1] = 0.0f;
}

} // namespace wave::engine::render::camera
