#pragma once

#include <cstdint>

#include "core/Vec2.h"

namespace si {

enum class EnemyKind : std::uint8_t {
    Top,   // linha superior: mais valor, sprite "aeronave"
    Mid,   // linhas centrais
    Low,   // linha inferior: menos valor, sprite "caranguejo"
};

std::int64_t scoreForKind(EnemyKind kind);

struct Enemy {
    Vec2 pos{};           // top-left no mundo
    EnemyKind kind = EnemyKind::Low;
    std::int64_t scoreValue = 0;
    bool alive = true;
    float animTimer = 0.0f;
};

}  // namespace si