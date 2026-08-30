#include "TestFramework.h"

#include <algorithm>

#include "core/Config.h"
#include "core/Rng.h"
#include "systems/EnemyFormation.h"
#include "systems/ProjectileManager.h"

using namespace si;

using Formation = EnemyFormation;

namespace {

FormationConfig defaultConfig() {
    FormationConfig fc;
    fc.baseSpeed = cfg::kFormationBaseSpeed;
    fc.descentStep = cfg::kFormationDescentStep;
    return fc;
}

}  // namespace

TEST(FormationSpawnCountAndOrder) {
    Formation f;
    f.reset(defaultConfig());
    CHECK(f.hasEnemies());
    CHECK(f.enemies().size() == static_cast<std::size_t>(cfg::kFormationCols * cfg::kFormationRows));

    // Linha superior = Top, última linha = Low.
    bool hasTop = false;
    bool hasLow = false;
    for (const auto& e : f.enemies()) {
        hasTop = hasTop || e.kind == EnemyKind::Top;
        hasLow = hasLow || e.kind == EnemyKind::Low;
    }
    CHECK(hasTop);
    CHECK(hasLow);
}

TEST(FormationMovesAndReverses) {
    Formation f;
    f.reset(defaultConfig());
    const float startLeft = f.minAliveX();

    // Movimento inicial para a direita (bounds direito cresce).
    float maxRight = 0.0f;
    for (int i = 0; i < 40; ++i) {
        f.update(1.0f / 60.0f);
        maxRight = std::max(maxRight, f.maxAliveX());
    }
    CHECK(maxRight >= startLeft);

    // Após encostar na borda direita, desce e inverte: a formação nunca pode
    // ultrapassar o limite direito permitido e termina dentro da tela.
    for (int i = 0; i < 600; ++i) {
        f.update(1.0f / 60.0f);
    }
    const float rightLimit = static_cast<float>(cfg::kLogicalWidth) - cfg::kBoundaryMargin;
    CHECK(f.maxAliveX() <= rightLimit + 1.0f);
    CHECK(f.minAliveX() < rightLimit);
}

TEST(FormationDescentOnReversal) {
    Formation f;
    f.reset(defaultConfig());
    const float startY = f.bottomY();
    for (int i = 0; i < 900; ++i) {
        f.update(1.0f / 60.0f);
    }
    CHECK(f.bottomY() > startY + 0.5f * cfg::kFormationDescentStep);
}

TEST(FormationClearsAndShoots) {
    Formation f;
    f.reset(defaultConfig());
    Rng rng(7);

    // Sem inimigos vivos, sem tiro pendente em intervalo grande.
    int shots = 0;
    for (int i = 0; i < 120; ++i) {
        shots += static_cast<int>(f.pollShots(rng, 0.01f, 1.0f / 60.0f).size());
    }
    CHECK(shots > 0);  // cadência alta gera tiros lentos mas reais

    for (auto& e : f.enemiesMut()) {
        e.alive = false;
    }
    CHECK(!f.hasEnemies());
    CHECK(f.pollShots(rng, 0.01f, 1.0f / 60.0f).empty());
}

TEST(FormationShootersFromLowestLiving) {
    // O atirador de cada coluna é o último vivo; tiros saem da linha real.
    Formation f;
    f.reset(defaultConfig());

    // Mata apenas a linha mais baixa; os tiros devem vir do pé da formação.
    float lowestStart = 0.0f;
    for (const auto& e : f.enemiesMut()) {
        if (e.kind == EnemyKind::Low) {
            lowestStart = std::max(lowestStart, e.pos.y + cfg::kEnemyHeight);
        }
    }
    for (auto& e : f.enemiesMut()) {
        if (e.kind == EnemyKind::Low) {
            e.alive = false;
        }
    }
    Rng rng(3);
    float minOriginY = 1e9f;
    for (int i = 0; i < 400; ++i) {
        for (const Vec2& shot : f.pollShots(rng, 0.02f, 1.0f / 60.0f)) {
            minOriginY = std::min(minOriginY, shot.y);
        }
    }
    CHECK(minOriginY < lowestStart + 1.0f);
}

TEST(ProjectilesLifeAndKill) {
    ProjectileManager pm;
    pm.addShot(Owner::Player, Vec2{10.0f, 100.0f}, Vec2{0.0f, -100.0f}, 4.0f, 14.0f, -1.0f);
    CHECK(pm.shots().size() == 1);
    CHECK(pm.hasPlayerShot());

    pm.addShot(Owner::Enemy, Vec2{20.0f, 20.0f}, Vec2{0.0f, 50.0f}, 8.0f, 16.0f, 7.0f);
    CHECK(pm.shots().size() == 2);

    // Depois de 7s o tiro inimigo expira pela vida; o do jogador já saiu da
    // tela pelo topo. Ambos sumiram do vetor (sweep interno do update).
    for (int i = 0; i < 60 * 7; ++i) {
        pm.update(1.0f / 60.0f, static_cast<float>(cfg::kLogicalHeight));
    }
    CHECK(pm.shots().empty());
    CHECK(!pm.hasPlayerShot());

    // Marcar morto não remove: só o sweep limpa.
    pm.addShot(Owner::Player, Vec2{10.0f, 10.0f}, Vec2{0.0f, -100.0f}, 4.0f, 14.0f, -1.0f);
    CHECK(pm.shots().size() == 1);
    CHECK(pm.shots()[0].dead == false);
    pm.kill(0);
    CHECK(pm.shots().size() == 1);
    CHECK(pm.shots()[0].dead == true);
    pm.sweepDead();
    CHECK(pm.shots().empty());
}

TEST(ProjectilesRemoveOffScreen) {
    ProjectileManager pm;
    // Já nasce fora da tela pelo topo (y < -40) e pelo fundo (y > altura+40).
    pm.addShot(Owner::Player, Vec2{10.0f, -50.0f}, Vec2{0.0f, -100.0f}, 4.0f, 14.0f, -1.0f);
    pm.addShot(Owner::Enemy, Vec2{10.0f, static_cast<float>(cfg::kLogicalHeight) + 50.0f}, Vec2{0.0f, 50.0f}, 8.0f, 16.0f, 7.0f);
    pm.update(0.01f, static_cast<float>(cfg::kLogicalHeight));
    CHECK(pm.shots().empty());
}
TEST(ProjectilesKillAllOfOwner) {
    ProjectileManager pm;
    pm.addShot(Owner::Player, Vec2{10.0f, 100.0f}, Vec2{0.0f, -100.0f}, 4.0f, 14.0f, -1.0f);
    pm.addShot(Owner::Enemy, Vec2{20.0f, 20.0f}, Vec2{0.0f, 50.0f}, 8.0f, 16.0f, 7.0f);
    pm.addShot(Owner::Enemy, Vec2{40.0f, 30.0f}, Vec2{0.0f, 50.0f}, 8.0f, 16.0f, 7.0f);
    CHECK(pm.count() == 3);

    // Marca sem varrer: os índices de quem estiver iterando seguem válidos.
    pm.killAllOf(Owner::Enemy);
    CHECK(pm.count() == 3);
    CHECK(pm.hasPlayerShot());

    pm.sweepDead();
    CHECK(pm.count() == 1);
    CHECK(pm.shots()[0].owner == Owner::Player);
}

TEST(FormationVolleyRespectsShooterCap) {
    Formation f;
    f.reset(defaultConfig());
    Rng rng(11);
    for (int i = 0; i < 200; ++i) {
        const auto volley = f.pollShots(rng, 0.001f, 1.0f / 60.0f);
        CHECK(static_cast<int>(volley.size()) <= Formation::kMaxShootersPerVolley);
    }
}

TEST(FormationEmptyIsInertButSafe) {
    Formation f;
    FormationConfig empty = defaultConfig();
    empty.cols = 0;
    empty.rows = 0;
    f.reset(empty);
    CHECK(f.totalCount() == 0);
    CHECK(!f.hasEnemies());

    Rng rng(5);
    f.update(1.0f / 60.0f);  // não pode dividir por zero nem travar
    CHECK(f.pollShots(rng, 0.5f, 1.0f / 60.0f).empty());
}
