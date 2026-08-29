#include "core/GameSession.h"

#include <algorithm>

#include "core/Rect.h"
#include "entities/Enemy.h"
#include "entities/Projectile.h"

namespace si {

namespace {

Color playerHitColor() { return rgb(255, 150, 40); }
Color shieldHitColor() { return rgb(120, 255, 130); }
Color enemyColorFor(EnemyKind kind) {
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

}  // namespace

GameSession::GameSession(std::uint64_t seed) : rng_(seed) {}

void GameSession::reset() {
    score_ = 0;
    player_.lives = cfg::kStartLives;
    level_ = 1;
    status_ = Status::Playing;
    startLevel(level_);
}

void GameSession::spawnShields() {
    shields_.clear();
    const float gap = (static_cast<float>(cfg::kLogicalWidth) - static_cast<float>(cfg::kShieldCount) * cfg::kShieldWidth)
                      / static_cast<float>(cfg::kShieldCount + 1);
    for (int i = 0; i < cfg::kShieldCount; ++i) {
        Shield s;
        s.spawn(Vec2{gap + static_cast<float>(i) * (cfg::kShieldWidth + gap), cfg::kShieldY});
        shields_.push_back(s);
    }
}

void GameSession::startLevel(int level) {
    level_ = level < 1 ? 1 : level;
    levelConfig_ = makeLevelConfig(level_);
    status_ = Status::Playing;

    FormationConfig fc;
    fc.baseSpeed = levelConfig_.formationSpeed;
    fc.descentStep = levelConfig_.formationDescent;
    formation_.reset(fc);

    shots_.clear();
    particles_.clear();
    spawnShields();

    const float spawnX = (static_cast<float>(cfg::kLogicalWidth) - player_.w) * 0.5f;
    player_.reset(Vec2{spawnX, cfg::kPlayerLineY}, player_.lives, 0.0f,
                  static_cast<float>(cfg::kLogicalWidth));
    player_.fireCooldown = 0.0f;
    player_.invulnTimer = cfg::kPlayerInvulnTime;  // respawn seguro
}

void GameSession::playerFire() {
    const Rect r = player_.rect();
    const Vec2 pos{centerX(r) - cfg::kPlayerShotWidth * 0.5f, r.y - cfg::kPlayerShotHeight};
    shots_.addShot(Owner::Player, pos, Vec2{0.0f, -cfg::kPlayerShotSpeed}, cfg::kPlayerShotWidth,
                   cfg::kPlayerShotHeight, -1.0f);
    player_.beginFireCooldown();
}

void GameSession::enemyFireFrom(Vec2 origin) {
    const Vec2 pos{origin.x - cfg::kEnemyShotWidth * 0.5f, origin.y};
    shots_.addShot(Owner::Enemy, pos, Vec2{0.0f, levelConfig_.enemyShotSpeed}, cfg::kEnemyShotWidth,
                   cfg::kEnemyShotHeight, cfg::kEnemyShotMaxLifetime);
}

void GameSession::addScore(std::int64_t points) {
    score_ += points;
    if (score_ > highScore_) {
        highScore_ = score_;
    }
}

void GameSession::playerHit() {
    player_.lives -= 1;
    player_.hit();
    particles_.emitBurst(center(player_.rect()), playerHitColor(), 26, 220.0f, 0.55f);

    // Tiro que matou some; tira também os inimigos em voo (fair play).
    {
        const auto& live = shots_.shots();
        for (std::size_t i = 0; i < live.size(); ++i) {
            if (live[i].owner == Owner::Enemy) {
                shots_.kill(i);
            }
        }
    }
    shots_.sweepDead();

    if (player_.lives <= 0) {
        status_ = Status::Lost;
    }
}

void GameSession::update(float dt, const GameInput& input) {
    if (status_ != Status::Playing) {
        return;
    }

    player_.update(dt, input.left, input.right);
    if (input.fire && player_.canFire() && !shots_.hasPlayerShot()) {
        playerFire();
    }

    formation_.update(dt);
    for (const Vec2& origin : formation_.pollShots(rng_, levelConfig_.enemyFireInterval, dt)) {
        enemyFireFrom(origin);
    }

    shots_.update(dt, static_cast<float>(cfg::kLogicalHeight));

    resolveCollisions();
    particles_.update(dt);
    checkEndConditions();
}

void GameSession::resolveCollisions() {
    const bool playerInvuln = player_.isInvulnerable();

    // Formação tocando a nave: a nave perde vida e o inimigo morre.
    if (!playerInvuln) {
        for (auto& e : formation_.enemiesMut()) {
            if (e.alive && overlaps(makeRect(e.pos, Vec2{cfg::kEnemyWidth, cfg::kEnemyHeight}), player_.rect())) {
                e.alive = false;
                addScore(e.scoreValue);
                particles_.emitBurst(center(makeRect(e.pos, Vec2{cfg::kEnemyWidth, cfg::kEnemyHeight})),
                                     enemyColorFor(e.kind), 12, 160.0f, 0.4f);
                playerHit();
                break;
            }
        }
    }

    auto& projectiles = shots_.shots();
    for (std::size_t i = 0; i < projectiles.size(); ++i) {
        auto& p = projectiles[i];
        if (p.dead) {
            continue;
        }
        const Rect pRect = makeRect(p.pos, Vec2{p.w, p.h});

        if (p.owner == Owner::Player) {
            // Tiro do jogador vs inimigos.
            bool consumed = false;
            for (auto& e : formation_.enemiesMut()) {
                if (!e.alive) {
                    continue;
                }
                const Rect eRect = makeRect(e.pos, Vec2{cfg::kEnemyWidth, cfg::kEnemyHeight});
                if (overlaps(pRect, eRect)) {
                    e.alive = false;
                    addScore(e.scoreValue);
                    particles_.emitBurst(center(eRect), enemyColorFor(e.kind), 16, 190.0f, 0.45f);
                    shots_.kill(i);
                    consumed = true;
                    break;
                }
            }
            if (consumed) {
                continue;
            }
            // Tiro do jogador vs barreiras.
            for (auto& shield : shields_) {
                if (shield.destroy(pRect)) {
                    particles_.emitBurst(center(pRect), shieldHitColor(), 6, 120.0f, 0.3f);
                    shots_.kill(i);
                    consumed = true;
                    break;
                }
            }
        } else {
            // Tiro inimigo vs nave.
            if (!playerInvuln && overlaps(pRect, player_.rect())) {
                playerHit();
                shots_.kill(i);
                continue;
            }
            // Tiro inimigo vs barreiras.
            for (auto& shield : shields_) {
                if (shield.destroy(pRect)) {
                    particles_.emitBurst(center(pRect), shieldHitColor(), 6, 120.0f, 0.3f);
                    shots_.kill(i);
                    break;
                }
            }
        }
    }
    shots_.sweepDead();
}

void GameSession::checkEndConditions() {
    if (status_ != Status::Playing) {
        return;
    }
    if (!formation_.hasEnemies()) {
        addScore(static_cast<std::int64_t>(cfg::kScoreBonusLevelClear) * level_);
        status_ = Status::Won;
        return;
    }
    // Formação invadiu a linha do jogador: derrota imediata.
    if (formation_.bottomY() >= player_.rect().y) {
        status_ = Status::Lost;
    }
}

}  // namespace si