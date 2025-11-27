#pragma once

#include "engine/core/math/vec3.hpp"
#include "engine/core/math/mat4.hpp"

namespace wave::engine::core::math
{
    // Basic transform: position, Euler rotation (radians), and scale.
    //
    // This is a lightweight utility, not tied to any ECS.
    // It is meant to be used by rendering, scene graph, editor gizmos, etc.
    struct Transform
    {
        Vec3 position { 0.0f, 0.0f, 0.0f };
        Vec3 rotation { 0.0f, 0.0f, 0.0f }; // Euler angles in radians: (pitch=X, yaw=Y, roll=Z)
        Vec3 scale    { 1.0f, 1.0f, 1.0f };

        Transform() = default;

        Transform(const Vec3& p, const Vec3& r, const Vec3& s)
            : position(p), rotation(r), scale(s)
        {
        }

        // Build a TRS matrix (Translation * Rotation * Scale).
        // Rotation order: X then Y then Z.
        Mat4 to_matrix() const noexcept
        {
            Mat4 t = Mat4::translation(position);
            Mat4 rx = Mat4::rotation_x(rotation.x);
            Mat4 ry = Mat4::rotation_y(rotation.y);
            Mat4 rz = Mat4::rotation_z(rotation.z);
            Mat4 s = Mat4::scale(scale);

            Mat4 r = rx * ry * rz;
            return t * r * s;
        }

        // Convenience: set identity transform.
        void set_identity() noexcept
        {
            position = {0.0f, 0.0f, 0.0f};
            rotation = {0.0f, 0.0f, 0.0f};
            scale    = {1.0f, 1.0f, 1.0f};
        }
    };

} // namespace wave::engine::core::math
