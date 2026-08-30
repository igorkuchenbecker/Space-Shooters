#include "TestFramework.h"

#include "core/Config.h"
#include "core/GameEvent.h"
#include "core/GameSession.h"
#include "entities/Enemy.h"

using namespace si;

namespace {

GameInput idle() {
    GameInput in;
    in.left = false;
    in.right = false;
    in.fire = false;
    return in;
}

// Joga N passos de simulação com entrada constante.
void run(GameSession& s, int fixedSteps, const GameInput& in) {
    for (int i = 0; i < fixedSteps; ++i) {
        s.update(1.0f / 60.0f, in);
        if (s.status() != GameSession::Status::Playing) {
            break;
        }
    }
}

bool hasEvent(const GameSession& s, GameEvent want) {
    for (const GameEvent e : s.events()) {
        if (e == want) {
            return true;
        }
    }
    return false;
}

}  // namespace

TEST(SessionResetBasics) {
    GameSession s(42);
    s.reset();
    CHECK(s.status() == GameSession::Status::Playing);
    CHECK(s.score() == 0);
    CHECK(s.lives() == cfg::kStartLives);
    CHECK(s.level() == 1);
    CHECK(!s.formation().enemies().empty());
    CHECK(s.shields().size() == static_cast<std::size_t>(cfg::kShieldCount));
    CHECK(s.player().isInvulnerable());  // respawn seguro no início
}

TEST(SessionStartLevelResetsField) {
    GameSession s(42);
    s.reset();
    s.startLevel(2);
    CHECK(s.level() == 2);
    CHECK(s.formation().hasEnemies());
    CHECK(s.projectiles().shots().empty());
    CHECK(s.player().isInvulnerable());
}

TEST(SessionSinglePlayerShot) {
    GameSession s(42);
    s.reset();
    GameInput fire = idle();
    fire.fire = true;
    s.update(1.0f / 60.0f, fire);
    s.update(1.0f / 60.0f, fire);
    CHECK(static_cast<int>(s.projectiles().shots().size()) == 1);
    CHECK(s.projectiles().hasPlayerShot());
}

TEST(SessionKillAllWinsWithBonus) {
    GameSession s(42);
    s.reset();
    CHECK(s.score() == 0);
    s.formation().killAll();
    s.update(1.0f / 60.0f, idle());
    CHECK(s.status() == GameSession::Status::Won);
    CHECK(s.score() == static_cast<std::int64_t>(cfg::kScoreBonusLevelClear));  // nível 1
}

TEST(SessionHigherLevelBiggerBonus) {
    GameSession s(42);
    s.reset();
    s.startLevel(3);
    s.formation().killAll();
    s.update(1.0f / 60.0f, idle());
    CHECK(s.status() == GameSession::Status::Won);
    CHECK(s.score() == static_cast<std::int64_t>(cfg::kScoreBonusLevelClear) * 3);
}

TEST(SessionFormationInvadesLoses) {
    GameSession s(42);
    s.reset();
    // Força a formação até a linha do jogador.
    for (auto& e : s.formation().enemiesMut()) {
        e.pos.y = cfg::kPlayerLineY;
    }
    s.update(1.0f / 60.0f, idle());
    CHECK(s.status() == GameSession::Status::Lost);
    (void)idle();
}

TEST(SessionPlayerHitReducesLives) {
    GameSession s(42);
    s.reset();
    s.player().invulnTimer = 0.0f;  // sem respawn seguro

    // Tiro inimigo colide com a nave.
    s.projectiles().addShot(Owner::Enemy, s.player().pos, Vec2{0.0f, 0.0f}, 8.0f, 16.0f, 7.0f);
    const int livesBefore = s.lives();
    s.update(1.0f / 60.0f, idle());
    CHECK(s.lives() == livesBefore - 1);
    CHECK(s.player().isInvulnerable());
}

TEST(SessionLoseAllLivesEndsPlay) {
    GameSession s(42);
    s.reset();
    s.player().invulnTimer = 0.0f;
    s.player().lives = 1;
    s.projectiles().addShot(Owner::Enemy, s.player().pos, Vec2{0.0f, 0.0f}, 8.0f, 16.0f, 7.0f);
    s.update(1.0f / 60.0f, idle());
    CHECK(s.status() == GameSession::Status::Lost);
    (void)run;
}

TEST(SessionHighScoreTracks) {
    GameSession s(42);
    s.reset();
    CHECK(s.highScore() == 0);
    s.setHighScore(500);
    CHECK(s.highScore() == 500);
    s.reset();
    CHECK(s.score() == 0);
    CHECK(s.highScore() == 500);  // persiste entre partidas na memória
}

TEST(SessionExtraLife) {
    GameSession s(42);
    s.reset();
    const int before = s.lives();
    s.grantExtraLife();
    CHECK(s.lives() == before + 1);
}
TEST(SessionExtraLifeOnScoreThreshold) {
    GameSession s(42);
    s.reset();
    const int before = s.lives();

    // Limpar o nível 1 com todos os inimigos abatidos ultrapassa a faixa de
    // vida extra (55 inimigos valem bem mais que kExtraLifeEvery).
    for (auto& e : s.formation().enemiesMut()) {
        e.alive = false;
    }
    s.update(1.0f / 60.0f, idle());
    CHECK(s.score() >= cfg::kExtraLifeEvery ? s.lives() > before : s.lives() == before);
}

TEST(SessionExtraLifeRespectsCap) {
    GameSession s(42);
    s.reset();
    s.player().lives = cfg::kMaxLives;
    for (auto& e : s.formation().enemiesMut()) {
        e.alive = false;
        e.scoreValue = cfg::kExtraLifeEvery;
    }
    s.update(1.0f / 60.0f, idle());
    CHECK(s.lives() == cfg::kMaxLives);  // nunca passa do teto
}

TEST(SessionEmitsGameplayEvents) {
    GameSession s(42);
    s.reset();
    s.clearEvents();

    GameInput fire = idle();
    fire.fire = true;
    s.update(1.0f / 60.0f, fire);
    CHECK(hasEvent(s, GameEvent::PlayerShot));

    s.clearEvents();
    CHECK(s.events().empty());
}

TEST(SessionEmitsEnemyKilledAndGameOver) {
    GameSession s(42);
    s.reset();
    s.clearEvents();
    s.formation().killAll();
    s.update(1.0f / 60.0f, idle());
    CHECK(hasEvent(s, GameEvent::LevelCleared));

    GameSession lost(42);
    lost.reset();
    lost.player().invulnTimer = 0.0f;
    lost.player().lives = 1;
    lost.clearEvents();
    lost.projectiles().addShot(Owner::Enemy, lost.player().pos, Vec2{0.0f, 0.0f}, 8.0f, 16.0f, 7.0f);
    lost.update(1.0f / 60.0f, idle());
    CHECK(hasEvent(lost, GameEvent::PlayerHit));
    CHECK(hasEvent(lost, GameEvent::GameOver));
}

// Regressão: `playerHit` costumava varrer o vetor de projéteis no meio do
// laço indexado de colisões, então o `kill(i)` seguinte atingia o projétil
// errado — o tiro do jogador sumia junto com o tiro inimigo que o acertou.
TEST(SessionPlayerHitKeepsPlayerShotAlive) {
    GameSession s(42);
    s.reset();
    s.player().invulnTimer = 0.0f;

    // Índice 0: tiro inimigo em cima da nave. Índice 1: tiro do jogador
    // subindo em um corredor vazio (acima da formação, longe dos escudos).
    s.projectiles().addShot(Owner::Enemy, s.player().pos, Vec2{0.0f, 0.0f}, 8.0f, 16.0f, 7.0f);
    s.projectiles().addShot(Owner::Player, Vec2{2.0f, 60.0f}, Vec2{0.0f, -cfg::kPlayerShotSpeed},
                            cfg::kPlayerShotWidth, cfg::kPlayerShotHeight, -1.0f);

    s.update(1.0f / 60.0f, idle());

    CHECK(s.lives() == cfg::kStartLives - 1);
    CHECK(s.projectiles().hasPlayerShot());  // o tiro do jogador continua vivo
    for (const auto& shot : s.projectiles().shots()) {
        CHECK(shot.owner == Owner::Player);  // todo tiro inimigo foi limpo
    }
}
