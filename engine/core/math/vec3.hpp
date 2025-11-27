#pragma once

#include <cmath>

namespace wave::engine::core::math
{
    struct Vec3
    {
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;

        Vec3() = default;
        Vec3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}

        Vec3 operator+(const Vec3& rhs) const noexcept { return {x + rhs.x, y + rhs.y, z + rhs.z}; }
        Vec3 operator-(const Vec3& rhs) const noexcept { return {x - rhs.x, y - rhs.y, z - rhs.z}; }
        Vec3 operator*(float s) const noexcept { return {x * s, y * s, z * s}; }
        Vec3 operator/(float s) const noexcept { return {x / s, y / s, z / s}; }

        Vec3& operator+=(const Vec3& rhs) noexcept { x += rhs.x; y += rhs.y; z += rhs.z; return *this; }
        Vec3& operator-=(const Vec3& rhs) noexcept { x -= rhs.x; y -= rhs.y; z -= rhs.z; return *this; }
        Vec3& operator*=(float s) noexcept { x *= s; y *= s; z *= s; return *this; }
        Vec3& operator/=(float s) noexcept { x /= s; y /= s; z /= s; return *this; }

        float length() const noexcept
        {
            return std::sqrt(x * x + y * y + z * z);
        }

        float length_squared() const noexcept
        {
            return x * x + y * y + z * z;
        }

        Vec3 normalized() const noexcept
        {
            float len = length();
            return len > 0.0f ? (*this / len) : Vec3{};
        }

        static float dot(const Vec3& a, const Vec3& b) noexcept
        {
            return a.x * b.x + a.y * b.y + a.z * b.z;
        }

        static Vec3 cross(const Vec3& a, const Vec3& b) noexcept
        {
            return {
                a.y * b.z - a.z * b.y,
                a.z * b.x - a.x * b.z,
                a.x * b.y - a.y * b.x
            };
        }
    };

} // namespace wave::engine::core::math
