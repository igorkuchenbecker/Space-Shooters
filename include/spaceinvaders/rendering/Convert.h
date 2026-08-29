#pragma once

#include "raylib.h"

#include "core/Color.h"
#include "core/Rect.h"
#include "core/Vec2.h"
#include "entities/Enemy.h"

namespace si {

inline ::Color toRay(Color c) { return {c.r, c.g, c.b, c.a}; }
inline ::Rectangle toRay(Rect r) { return {r.x, r.y, r.w, r.h}; }
inline ::Vector2 toRay(Vec2 v) { return {v.x, v.y}; }

inline ::Color enemyColorFor(EnemyKind kind) {
    switch (kind) {
        case EnemyKind::Top:
            return {255, 70, 70, 255};
        case EnemyKind::Mid:
            return {255, 226, 100, 255};
        case EnemyKind::Low:
            return {95, 214, 255, 255};
    }
    return {255, 255, 255, 255};
}

// Paleta visual do jogo (uma única fonte de cor, sem magic numbers no render).
struct Palette {
    Color background = Color{7, 6, 18, 255};
    Color border = Color{20, 18, 46, 255};
    Color text = Color{238, 238, 255, 255};
    Color textDim = Color{150, 150, 190, 255};
    Color accent = Color{255, 205, 60, 255};
    Color player = Color{156, 255, 95, 255};
    Color shield = Color{74, 255, 122, 255};
    Color playerShot = Color{255, 244, 120, 255};
    Color enemyShot = Color{255, 130, 90, 255};
};

inline const Palette& palette() {
    static const Palette p;
    return p;
}

}  // namespace si