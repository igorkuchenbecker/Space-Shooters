#include "TestFramework.h"

#include "core/Config.h"
#include "entities/Enemy.h"
#include "entities/Particle.h"
#include "entities/Player.h"
#include "entities/Projectile.h"
#include "entities/Shield.h"

using namespace si;

TEST(EnemyScores) {
    CHECK(scoreForKind(EnemyKind::Top) == 300);
    CHECK(scoreForKind(EnemyKind::Mid) == 200);
    CHECK(scoreForKind(EnemyKind::Low) == 100);
}

TEST(PlayerMovementClamped) {
    Player p;
    p.reset(Vec2{100.0f, cfg::kPlayerLineY}, 3, 0.0f, static_cast<float>(cfg::kLogicalWidth));
    CHECK(p.lives == 3);
    CHECK(p.pos.x >= 0.0f);

    const float before = p.pos.x;
    p.update(1.0f / 60.0f, true, false);
    CHECK(p.pos.x < before);

    // Segura esquerda por muito tempo: nunca passa do limite.
    for (int i = 0; i < 200; ++i) {
        p.update(1.0f / 60.0f, true, false);
    }
    CHECK(p.pos.x >= p.minX);
    CHECK(p.moving);

    // Segura direita por muito tempo: nunca passa do limite.
    for (int i = 0; i < 400; ++i) {
        p.update(1.0f / 60.0f, false, true);
    }
    CHECK(p.pos.x <= p.maxX);

    p.update(1.0f / 60.0f, false, false);
    CHECK(!p.moving);
}

TEST(PlayerFireCooldownAndInvuln) {
    Player p;
    p.reset(Vec2{0.0f, 0.0f}, 3, 0.0f, 100.0f);
    CHECK(p.canFire());
    p.beginFireCooldown();
    CHECK(!p.canFire());
    p.update(cfg::kPlayerFireCooldown, false, false);
    CHECK(p.canFire());

    CHECK(!p.isInvulnerable());
    p.hit();
    CHECK(p.isInvulnerable());
    p.update(cfg::kPlayerInvulnTime, false, false);
    CHECK(!p.isInvulnerable());
}

TEST(ProjectileOwnership) {
    Projectile p;
    CHECK(p.owner == Owner::Player);
    CHECK(!p.dead);
    CHECK(p.life == 1e9f);
}

TEST(ShieldSpawnAndDome) {
    Shield s;
    s.spawn(Vec2{100.0f, 400.0f});
    CHECK(s.origin().x == 100.0f);
    CHECK(s.blocksLeft() > 0);
    // O desenho em "dome" não nasce 100% preenchido (colunas internas vazias).
    CHECK(s.blocksLeft() < s.kCols * s.kRows);

    // Formato padrão: a fileira superior é a mais larga do domo.
    int topAlive = 0;
    int bodyAlive = 0;
    for (int c = 0; c < s.kCols; ++c) {
        if (s.blockAlive(c, 0)) {
            ++topAlive;
        }
        if (s.blockAlive(c, s.kRows / 2)) {
            ++bodyAlive;
        }
    }
    CHECK(topAlive == s.kCols);  // fileira 0 sempre cheia
    CHECK(topAlive > bodyAlive);
    CHECK(topAlive > 0);
}

TEST(ShieldDestroyAndEmpty) {
    Shield s;
    s.spawn(Vec2{0.0f, 0.0f});
    const int total = s.blocksLeft();

    // Um tiro que cobre todo o topo quebra só a coluna sob ele.
    const bool broke = s.destroy(makeRect(Vec2{8.0f, 0.0f}, Vec2{16.0f, 4.0f}));
    CHECK(broke);
    CHECK(s.blocksLeft() < total);
    CHECK(!s.blockAlive(1, 0));

    // Quebra tudo resto bloco a bloco.
    for (int r = 0; r < s.kRows; ++r) {
        for (int c = 0; c < s.kCols; ++c) {
            if (s.blockAlive(c, r)) {
                (void)s.destroy(s.blockRect(c, r));
            }
        }
    }
    CHECK(s.blocksLeft() == 0);
    CHECK(s.empty());
    CHECK(!s.destroy(makeRect(Vec2{0.0f, 0.0f}, Vec2{40.0f, 40.0f})));  // nada a quebrar
}

TEST(ParticleSystemBurstAndSweep) {
    ParticleSystem fx;
    CHECK(fx.particles().empty());
    fx.emitBurst(Vec2{10.0f, 10.0f}, rgb(255, 255, 255), 20, 100.0f, 0.5f);
    CHECK(fx.particles().size() == 20);

    // Depois do tempo de vida todos morrem e são removidos no update.
    float t = 0.0f;
    while (t < 1.0f) {
        fx.update(0.1f);
        t += 0.1f;
    }
    CHECK(fx.particles().empty());
}

TEST(ParticleCapacityCapped) {
    ParticleSystem fx;
    fx.emitBurst(Vec2{0.0f, 0.0f}, rgb(255, 255, 255), 3000, 50.0f, 1.0f);
    CHECK(fx.particles().size() <= fx.kMax);
}