#pragma once

#include <cstdint>
#include <vector>

#include "core/Color.h"
#include "core/Config.h"
#include "core/GameEvent.h"
#include "core/LevelConfig.h"
#include "core/Rng.h"
#include "entities/Particle.h"
#include "entities/Player.h"
#include "entities/Shield.h"
#include "systems/EnemyFormation.h"
#include "systems/ProjectileManager.h"

namespace si {

// Entrada de um frame de simulação. O código de gameplay é puro: a camada de
// apresentação (raylib) só traduz teclado/gamepad para este struct.
struct GameInput {
    bool left = false;
    bool right = false;
    bool fire = false;
};

// Orquestra uma partida inteira: formação, jogador, projéteis, barreiras,
// partículas, pontuação, vidas e transição de nível. Não conhece raylib —
// é inteiramente testável.
class GameSession {
public:
    enum class Status : std::uint8_t { Playing, Won, Lost };

    explicit GameSession(std::uint64_t seed);

    // Partida nova: zera pontuação/vidas e começa no nível 1.
    void reset();

    // Reinicia o campo para o nível dado, preservando pontuação e vidas.
    void startLevel(int level);

    void update(float dt, const GameInput& input);

    [[nodiscard]] Status status() const { return status_; }

    [[nodiscard]] int level() const { return level_; }
    [[nodiscard]] std::int64_t score() const { return score_; }
    [[nodiscard]] std::int64_t highScore() const { return highScore_; }
    [[nodiscard]] int lives() const { return player_.lives; }

    void setHighScore(std::int64_t value) { highScore_ = value > highScore_ ? value : highScore_; }
    void grantExtraLife() { player_.lives += 1; }

    // Eventos produzidos desde a última leitura; `clearEvents` esvazia a fila.
    [[nodiscard]] const std::vector<GameEvent>& events() const { return events_; }
    void clearEvents() { events_.clear(); }

    [[nodiscard]] Player& player() { return player_; }
    [[nodiscard]] const Player& player() const { return player_; }
    [[nodiscard]] EnemyFormation& formation() { return formation_; }
    [[nodiscard]] const EnemyFormation& formation() const { return formation_; }
    [[nodiscard]] ProjectileManager& projectiles() { return shots_; }
    [[nodiscard]] const ProjectileManager& projectiles() const { return shots_; }
    [[nodiscard]] const std::vector<Shield>& shields() const { return shields_; }
    [[nodiscard]] ParticleSystem& particles() { return particles_; }
    [[nodiscard]] const ParticleSystem& particles() const { return particles_; }

private:
    Rng rng_;
    LevelConfig levelConfig_;
    Player player_;
    EnemyFormation formation_;
    ProjectileManager shots_;
    std::vector<Shield> shields_;
    ParticleSystem particles_;
    std::vector<GameEvent> events_;
    int level_ = 1;
    std::int64_t score_ = 0;
    std::int64_t highScore_ = 0;
    std::int64_t nextExtraLifeAt_ = cfg::kExtraLifeEvery;
    Status status_ = Status::Playing;

    void emit(GameEvent event) { events_.push_back(event); }

    void spawnShields();
    void playerFire();
    void enemyFireFrom(Vec2 origin);
    void killEnemy(Enemy& enemy);
    void playerHit();
    void resolveCollisions();
    void checkEndConditions();

    void addScore(std::int64_t points);
};

}  // namespace si
