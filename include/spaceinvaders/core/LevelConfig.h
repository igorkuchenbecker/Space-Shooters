#pragma once

#include "Config.h"

namespace si {

// Configuração de dificuldade derivada do nível; única fonte de constantes
// de gameplay por nível (sem magic numbers espalhados).
struct LevelConfig {
    int level = 1;
    float formationSpeed = cfg::kFormationBaseSpeed;
    float formationDescent = cfg::kFormationDescentStep;
    float enemyFireInterval = cfg::kEnemyFireBaseInterval;
    float enemyShotSpeed = cfg::kEnemyShotBaseSpeed;

    [[nodiscard]] float formationSpeedMultiplier() const { return formationSpeed / cfg::kFormationBaseSpeed; }
};

// Progressão de dificuldade: nível 1 é o baseline; cada nível seguinte
// acelera a formação, os tiros inimigos e a cadência de disparo, com tetos
// para o jogo não ficar impossível.
inline LevelConfig makeLevelConfig(int level) {
    const int lv = level < 1 ? 1 : level;
    LevelConfig c;
    c.level = lv;
    const float speedMult = 1.0f + cfg::kLevelSpeedFactor * static_cast<float>(lv - 1);
    c.formationSpeed = cfg::kFormationBaseSpeed * (speedMult > cfg::kLevelSpeedCap ? cfg::kLevelSpeedCap : speedMult);
    const float shotSpeed = cfg::kEnemyShotBaseSpeed + cfg::kLevelShotFactor * static_cast<float>(lv - 1);
    c.enemyShotSpeed = shotSpeed > cfg::kLevelShotCap ? cfg::kLevelShotCap : shotSpeed;
    const float fire = cfg::kEnemyFireBaseInterval - cfg::kLevelFireFactor * static_cast<float>(lv - 1);
    c.enemyFireInterval = fire < cfg::kLevelFireFloor ? cfg::kLevelFireFloor : fire;
    c.formationDescent = cfg::kFormationDescentStep + 2.0f * static_cast<float>(lv - 1);
    if (c.formationDescent > 44.0f) {
        c.formationDescent = 44.0f;
    }
    return c;
}

}  // namespace si