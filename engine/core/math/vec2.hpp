#pragma once

#include <cmath>

namespace wave::engine::core::math
{
    struct Vec2
    {
        float x = 0.0f;
        float y = 0.0f;

        Vec2() = default;
        Vec2(float x_, float y_) : x(x_), y(y_) {}

        // Basic ops
        Vec2 operator+(const Vec2& rhs) const noexcept { return {x + rhs.x, y + rhs.y}; }
        Vec2 operator-(const Vec2& rhs) const noexcept { return {x - rhs.x, y - rhs.y}; }
        Vec2 operator*(float s) const noexcept { return {x * s, y * s}; }
        Vec2 operator/(float s) const noexcept { return {x / s, y / s}; }

        Vec2& operator+=(const Vec2& rhs) noexcept { x += rhs.x; y += rhs.y; return *this; }
        Vec2& operator-=(const Vec2& rhs) noexcept { x -= rhs.x; y -= rhs.y; return *this; }
        Vec2& operator*=(float s) noexcept { x *= s; y *= s; return *this; }
        Vec2& operator/=(float s) noexcept { x /= s; y /= s; return *this; }

        float length() const noexcept
        {
            return std::sqrt(x * x + y * y);
        }

        float length_squared() const noexcept
        {
            return x * x + y * y;
        }

        Vec2 normalized() const noexcept
        {
            float len = length();
            return len > 0.0f ? (*this / len) : Vec2{};
        }

        static float dot(const Vec2& a, const Vec2& b) noexcept
        {
            return a.x * b.x + a.y * b.y;
        }
    };

} // namespace wave::engine::core::math
