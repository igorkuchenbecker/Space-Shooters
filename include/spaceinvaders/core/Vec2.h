#pragma once

namespace si {

struct Vec2 {
    float x = 0.0f;
    float y = 0.0f;
};

inline constexpr Vec2 operator+(Vec2 a, Vec2 b) { return {a.x + b.x, a.y + b.y}; }
inline constexpr Vec2 operator-(Vec2 a, Vec2 b) { return {a.x - b.x, a.y - b.y}; }
inline constexpr Vec2 operator*(Vec2 a, float s) { return {a.x * s, a.y * s}; }
inline constexpr Vec2 operator*(float s, Vec2 a) { return a * s; }
inline constexpr Vec2& operator+=(Vec2& a, Vec2 b) {
    a.x += b.x;
    a.y += b.y;
    return a;
}
inline constexpr bool operator==(Vec2 a, Vec2 b) { return a.x == b.x && a.y == b.y; }

}  // namespace si