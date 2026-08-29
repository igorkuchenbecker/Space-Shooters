#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

#include "core/Config.h"
#include "core/Rect.h"
#include "core/Rng.h"
#include "core/Vec2.h"
#include "entities/Enemy.h"

namespace si {

struct FormationConfig {
    int cols = cfg::kFormationCols;
    int rows = cfg::kFormationRows;
    float spacingX = cfg::kFormationSpacingX;
    float spacingY = cfg::kFormationSpacingY;
    float startY = cfg::kFormationStartY;
    float leftBound = cfg::kBoundaryMargin;
    float rightBound = static_cast<float>(cfg::kLogicalWidth) - cfg::kBoundaryMargin;
    float baseSpeed = cfg::kFormationBaseSpeed;
    float descentStep = cfg::kFormationDescentStep;
};

// Movimento coerente da formação como um todo: anda para o lado, inverte no
// limite, desce, e acelera conforme os inimigos morrem.
class EnemyFormation {
public:
    void reset(const FormationConfig& config);

    // Avança a formação. Retorna false quando não há mais inimigos vivos.
    void update(float dt);

    // Devolve os pontos de origem de tiro (borda inferior do atirador) quando
    // o temporizador de disparo vence; caso contrário, vetor vazio.
    [[nodiscard]] std::vector<Vec2> pollShots(Rng& rng, float fireInterval, float dt);

    [[nodiscard]] int aliveCount() const;
    [[nodiscard]] int totalCount() const;
    [[nodiscard]] bool hasEnemies() const;

    [[nodiscard]] const std::vector<Enemy>& enemies() const { return enemies_; }
    [[nodiscard]] std::vector<Enemy>& enemiesMut() { return enemies_; }

    void killAt(std::size_t index);
    void killAll();

    // Limites da formação (apenas inimigos vivos).
    [[nodiscard]] float minAliveX() const;
    [[nodiscard]] float maxAliveX() const;
    [[nodiscard]] float bottomY() const;

    [[nodiscard]] int direction() const { return direction_; }

private:
    FormationConfig cfg_;
    std::vector<Enemy> enemies_;
    int direction_ = 1;
    float fireTimer_ = 1.5f;

    void shiftHorizontally(float delta);
    void descend(float amount);
    [[nodiscard]] float effectiveSpeed() const;
};

}  // namespace si