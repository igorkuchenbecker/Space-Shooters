#include "TestFramework.h"

#include "core/Config.h"
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