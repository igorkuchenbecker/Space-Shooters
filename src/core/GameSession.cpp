#include "core/GameSession.h"

#include <algorithm>

#include "core/Rect.h"
#include "entities/Enemy.h"
#include "entities/Projectile.h"

namespace si {

namespace {

Color playerHitColor() { return rgb(255, 150, 40); }
Color shieldHitColor() { return rgb(120, 255, 130); }

Rect enemyRect(const Enemy& e) { return makeRect(e.pos, Vec2{cfg::kEnemyWidth, cfg::kEnemyHeight}); }

}  // namespace

GameSession::GameSession(std::uint64_t seed) : rng_(seed) {}

void GameSession::reset() {
    score_ = 0;
    nextExtraLifeAt_ = cfg::kExtraLifeEvery;
    player_.lives = cfg::kStartLives;
    level_ = 1;
    status_ = Status::Playing;
    events_.clear();
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
    emit(GameEvent::PlayerShot);
}

void GameSession::enemyFireFrom(Vec2 origin) {
    const Vec2 pos{origin.x - cfg::kEnemyShotWidth * 0.5f, origin.y};
    shots_.addShot(Owner::Enemy, pos, Vec2{0.0f, levelConfig_.enemyShotSpeed}, cfg::kEnemyShotWidth,
                   cfg::kEnemyShotHeight, cfg::kEnemyShotMaxLifetime);
    emit(GameEvent::EnemyShot);
}

void GameSession::addScore(std::int64_t points) {
    score_ += points;
    if (score_ > highScore_) {
        highScore_ = score_;
    }
    // Vida extra por faixa de pontuação; o laço cobre um salto grande de uma vez.
    while (score_ >= nextExtraLifeAt_) {
        nextExtraLifeAt_ += cfg::kExtraLifeEvery;
        if (player_.lives < cfg::kMaxLives) {
            grantExtraLife();
            emit(GameEvent::ExtraLife);
        }
    }
}

void GameSession::killEnemy(Enemy& enemy) {
    enemy.alive = false;
    particles_.emitBurst(center(enemyRect(enemy)), colorForKind(enemy.kind), 16, 190.0f, 0.45f);
    emit(GameEvent::EnemyKilled);
    addScore(enemy.scoreValue);
}

void GameSession::playerHit() {
    player_.lives -= 1;
    player_.hit();
    particles_.emitBurst(center(player_.rect()), playerHitColor(), 26, 220.0f, 0.55f);
    emit(GameEvent::PlayerHit);

    // Tira também os tiros inimigos em voo (fair play no respawn). Só marca:
    // quem varre o vetor é o `sweepDead` no fim de `resolveCollisions`, para
    // não invalidar os índices do laço que pode estar em andamento.
    shots_.killAllOf(Owner::Enemy);

    if (player_.lives <= 0) {
        status_ = Status::Lost;
        emit(GameEvent::GameOver);
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
    // Nenhum `sweepDead` até o fim: o laço abaixo indexa o vetor de projéteis,
    // e remover elementos no meio invalidaria os índices.
    if (!player_.isInvulnerable()) {
        // Formação tocando a nave: a nave perde vida e o inimigo morre.
        for (auto& e : formation_.enemiesMut()) {
            if (e.alive && overlaps(enemyRect(e), player_.rect())) {
                killEnemy(e);
                playerHit();
                break;
            }
        }
    }

    const auto& projectiles = shots_.shots();
    for (std::size_t i = 0; i < projectiles.size(); ++i) {
        const Projectile& p = projectiles[i];
        if (p.dead) {
            continue;
        }
        const Rect pRect = makeRect(p.pos, Vec2{p.w, p.h});
        const Owner owner = p.owner;

        if (owner == Owner::Player) {
            // Tiro do jogador vs inimigos.
            bool consumed = false;
            for (auto& e : formation_.enemiesMut()) {
                if (e.alive && overlaps(pRect, enemyRect(e))) {
                    killEnemy(e);
                    shots_.kill(i);
                    consumed = true;
                    break;
                }
            }
            if (consumed) {
                continue;
            }
        } else if (!player_.isInvulnerable() && overlaps(pRect, player_.rect())) {
            // Tiro inimigo vs nave: `playerHit` marca os tiros inimigos como
            // mortos, incluindo este.
            playerHit();
            continue;
        }

        // Qualquer tiro vs barreiras.
        for (auto& shield : shields_) {
            if (shield.destroy(pRect)) {
                particles_.emitBurst(center(pRect), shieldHitColor(), 6, 120.0f, 0.3f);
                emit(GameEvent::ShieldChipped);
                shots_.kill(i);
                break;
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
        emit(GameEvent::LevelCleared);
        return;
    }
    // Formação invadiu a linha do jogador: derrota imediata.
    if (formation_.bottomY() >= player_.rect().y) {
        status_ = Status::Lost;
        emit(GameEvent::GameOver);
    }
}

}  // namespace si
