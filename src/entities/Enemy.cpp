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

}  // namespace si