#pragma once

#include <cmath>

#include "engine/core/math/vec3.hpp"

namespace wave::engine::core::math
{
    // Simple 4x4 matrix.
    //
    // Stored row-major as m[row][col].
    // All helpers are written to be self consistent inside Wave Engine.
    struct Mat4
    {
        float m[4][4]{};

        // Identity matrix
        static Mat4 identity() noexcept
        {
            Mat4 result{};
            result.m[0][0] = 1.0f;
            result.m[1][1] = 1.0f;
            result.m[2][2] = 1.0f;
            result.m[3][3] = 1.0f;
            return result;
        }

        // Translation matrix
        static Mat4 translation(const Vec3& t) noexcept
        {
            Mat4 result = identity();
            result.m[3][0] = t.x;
            result.m[3][1] = t.y;
            result.m[3][2] = t.z;
            return result;
        }

        // Uniform or non-uniform scale
        static Mat4 scale(const Vec3& s) noexcept
        {
            Mat4 result{};
            result.m[0][0] = s.x;
            result.m[1][1] = s.y;
            result.m[2][2] = s.z;
            result.m[3][3] = 1.0f;
            return result;
        }

        // Rotation around X axis (radians)
        static Mat4 rotation_x(float radians) noexcept
        {
            Mat4 result = identity();
            float c = std::cos(radians);
            float s = std::sin(radians);

            result.m[1][1] = c;
            result.m[1][2] = s;
            result.m[2][1] = -s;
            result.m[2][2] = c;
            return result;
        }

        // Rotation around Y axis (radians)
        static Mat4 rotation_y(float radians) noexcept
        {
            Mat4 result = identity();
            float c = std::cos(radians);
            float s = std::sin(radians);

            result.m[0][0] = c;
            result.m[0][2] = -s;
            result.m[2][0] = s;
            result.m[2][2] = c;
            return result;
        }

        // Rotation around Z axis (radians)
        static Mat4 rotation_z(float radians) noexcept
        {
            Mat4 result = identity();
            float c = std::cos(radians);
            float s = std::sin(radians);

            result.m[0][0] = c;
            result.m[0][1] = s;
            result.m[1][0] = -s;
            result.m[1][1] = c;
            return result;
        }

        // Matrix multiplication
        Mat4 operator*(const Mat4& rhs) const noexcept
        {
            Mat4 result{};

            for (int row = 0; row < 4; ++row)
            {
                for (int col = 0; col < 4; ++col)
                {
                    result.m[row][col] =
                        m[row][0] * rhs.m[0][col] +
                        m[row][1] * rhs.m[1][col] +
                        m[row][2] * rhs.m[2][col] +
                        m[row][3] * rhs.m[3][col];
                }
            }

            return result;
        }

        Mat4& operator*=(const Mat4& rhs) noexcept
        {
            *this = (*this) * rhs;
            return *this;
        }

        // Transform a Vec3 as a position (w = 1)
        Vec3 transform_point(const Vec3& v) const noexcept
        {
            float x = v.x * m[0][0] + v.y * m[1][0] + v.z * m[2][0] + m[3][0];
            float y = v.x * m[0][1] + v.y * m[1][1] + v.z * m[2][1] + m[3][1];
            float z = v.x * m[0][2] + v.y * m[1][2] + v.z * m[2][2] + m[3][2];
            float w = v.x * m[0][3] + v.y * m[1][3] + v.z * m[2][3] + m[3][3];

            if (w != 0.0f)
            {
                float inv_w = 1.0f / w;
                return {x * inv_w, y * inv_w, z * inv_w};
            }

            return {x, y, z};
        }

        // Transform a Vec3 as a direction (ignores translation, w = 0)
        Vec3 transform_direction(const Vec3& v) const noexcept
        {
            float x = v.x * m[0][0] + v.y * m[1][0] + v.z * m[2][0];
            float y = v.x * m[0][1] + v.y * m[1][1] + v.z * m[2][1];
            float z = v.x * m[0][2] + v.y * m[1][2] + v.z * m[2][2];
            return {x, y, z};
        }

        // Perspective projection matrix (right handed, depth in [0, 1] or [-1, 1]
        // depending how you later configure your Vulkan / graphics backend).
        static Mat4 perspective(float fov_radians,
                                float aspect,
                                float z_near,
                                float z_far) noexcept
        {
            float f = 1.0f / std::tan(fov_radians * 0.5f);
            Mat4 result{};

            result.m[0][0] = f / aspect;
            result.m[1][1] = f;
            result.m[2][2] = z_far / (z_far - z_near);
            result.m[2][3] = 1.0f;
            result.m[3][2] = (-z_near * z_far) / (z_far - z_near);

            return result;
        }

        // Orthographic projection matrix
        static Mat4 orthographic(float left,
                                 float right,
                                 float bottom,
                                 float top,
                                 float z_near,
                                 float z_far) noexcept
        {
            Mat4 result = identity();

            result.m[0][0] = 2.0f / (right - left);
            result.m[1][1] = 2.0f / (top - bottom);
            result.m[2][2] = 1.0f / (z_far - z_near);

            result.m[3][0] = -(right + left) / (right - left);
            result.m[3][1] = -(top + bottom) / (top - bottom);
            result.m[3][2] = -z_near / (z_far - z_near);

            return result;
        }
    };

} // namespace wave::engine::core::math
