#pragma once

#include "Vec2.h"

namespace si {

struct Rect {
    float x = 0.0f;
    float y = 0.0f;
    float w = 0.0f;
    float h = 0.0f;
};

inline constexpr Rect makeRect(float x, float y, float w, float h) { return {x, y, w, h}; }
inline constexpr Rect makeRect(Vec2 pos, Vec2 size) { return {pos.x, pos.y, size.x, size.y}; }

inline constexpr Vec2 position(Rect r) { return {r.x, r.y}; }
inline constexpr Vec2 size(Rect r) { return {r.w, r.h}; }
inline constexpr Vec2 center(Rect r) { return {r.x + r.w * 0.5f, r.y + r.h * 0.5f}; }
inline constexpr Vec2 topLeft(Rect r) { return {r.x, r.y}; }
inline constexpr float centerX(Rect r) { return r.x + r.w * 0.5f; }
inline constexpr float centerY(Rect r) { return r.y + r.h * 0.5f; }

inline constexpr bool overlaps(const Rect& a, const Rect& b) {
    return a.x < b.x + b.w && a.x + a.w > b.x && a.y < b.y + b.h && a.y + a.h > b.y;
}

inline constexpr bool contains(Rect r, Vec2 p) {
    return p.x >= r.x && p.x <= r.x + r.w && p.y >= r.y && p.y <= r.y + r.h;
}

}  // namespace si