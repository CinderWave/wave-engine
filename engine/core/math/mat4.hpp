#pragma once

#include <cmath>

namespace wave::engine::math {

struct Vec3 {
    float x{0.0f};
    float y{0.0f};
    float z{0.0f};
};

struct Mat4 {
    // Column-major 4x4 (matches GLSL / Vulkan conventions)
    float m[16]{};

    static Mat4 identity() {
        Mat4 r{};
        r.m[0]  = 1.0f;
        r.m[5]  = 1.0f;
        r.m[10] = 1.0f;
        r.m[15] = 1.0f;
        return r;
    }

    static Mat4 perspective(float fovYRadians, float aspect, float zNear, float zFar) {
        // Right-handed, clip space with depth [0,1] (Vulkan style)
        const float f = 1.0f / std::tan(fovYRadians * 0.5f);

        Mat4 r{};
        r.m[0]  = f / aspect;
        r.m[5]  = f;
        r.m[10] = zFar / (zFar - zNear);
        r.m[11] = 1.0f;
        r.m[14] = (-zNear * zFar) / (zFar - zNear);
        // Others remain 0
        return r;
    }

    static Mat4 multiply(const Mat4& a, const Mat4& b) {
        Mat4 r{};

        // Column-major multiplication: r = a * b
        for (int c = 0; c < 4; ++c) {
            for (int rIdx = 0; rIdx < 4; ++rIdx) {
                r.m[c * 4 + rIdx] =
                    a.m[0 * 4 + rIdx] * b.m[c * 4 + 0] +
                    a.m[1 * 4 + rIdx] * b.m[c * 4 + 1] +
                    a.m[2 * 4 + rIdx] * b.m[c * 4 + 2] +
                    a.m[3 * 4 + rIdx] * b.m[c * 4 + 3];
            }
        }

        return r;
    }

    static Mat4 look_at(const Vec3& eye, const Vec3& target, const Vec3& up) {
        // Standard right-handed look-at.
        Vec3 f{
            target.x - eye.x,
            target.y - eye.y,
            target.z - eye.z
        };

        // Normalize f
        {
            float len = std::sqrt(f.x * f.x + f.y * f.y + f.z * f.z);
            if (len > 0.0f) {
                f.x /= len;
                f.y /= len;
                f.z /= len;
            }
        }

        // s = normalize(cross(f, up))
        Vec3 s{
            f.y * up.z - f.z * up.y,
            f.z * up.x - f.x * up.z,
            f.x * up.y - f.y * up.x
        };
        {
            float len = std::sqrt(s.x * s.x + s.y * s.y + s.z * s.z);
            if (len > 0.0f) {
                s.x /= len;
                s.y /= len;
                s.z /= len;
            }
        }

        // u = cross(s, f)
        Vec3 u{
            s.y * f.z - s.z * f.y,
            s.z * f.x - s.x * f.z,
            s.x * f.y - s.y * f.x
        };

        Mat4 r = Mat4::identity();

        // Rotation (columns)
        r.m[0] =  s.x;
        r.m[4] =  s.y;
        r.m[8] =  s.z;

        r.m[1] =  u.x;
        r.m[5] =  u.y;
        r.m[9] =  u.z;

        r.m[2]  = -f.x;
        r.m[6]  = -f.y;
        r.m[10] = -f.z;

        // Translation
        r.m[12] = -(s.x * eye.x + s.y * eye.y + s.z * eye.z);
        r.m[13] = -(u.x * eye.x + u.y * eye.y + u.z * eye.z);
        r.m[14] =  (f.x * eye.x + f.y * eye.y + f.z * eye.z);

        return r;
    }
};

} // namespace wave::engine::math
