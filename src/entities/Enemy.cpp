#include "entities/Enemy.h"

#include "core/Config.h"

namespace si {

std::int64_t scoreForKind(EnemyKind kind) {
    switch (kind) {
        case EnemyKind::Top:
            return cfg::kScoreTopRow;
        case EnemyKind::Mid:
            return cfg::kScoreMidRow;
        case EnemyKind::Low:
            return cfg::kScoreLowRow;
    }
    return 0;
}

Color colorForKind(EnemyKind kind) {
    switch (kind) {
        case EnemyKind::Top:
            return rgb(255, 70, 70);
        case EnemyKind::Mid:
            return rgb(255, 226, 100);
        case EnemyKind::Low:
            return rgb(95, 214, 255);
    }
    return rgb(255, 255, 255);
}

}  // namespace si
