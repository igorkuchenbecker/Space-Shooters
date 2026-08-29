#include "entities/Player.h"

#include <algorithm>

namespace si {

void Player::reset(Vec2 spawnPos, int startLives, float rangeMinX, float rangeMaxX) {
    pos = spawnPos;
    lives = startLives;
    fireCooldown = 0.0f;
    invulnTimer = 0.0f;
    moving = false;
    minX = rangeMinX;
    maxX = rangeMaxX;
    pos.x = std::clamp(spawnPos.x, minX, maxX - w);
}

void Player::update(float dt, bool left, bool right) {
    fireCooldown = std::max(0.0f, fireCooldown - dt);
    invulnTimer = std::max(0.0f, invulnTimer - dt);

    const float dir = (right ? 1.0f : 0.0f) - (left ? 1.0f : 0.0f);
    moving = dir != 0.0f;
    if (moving) {
        pos.x += dir * cfg::kPlayerSpeed * dt;
    }
    pos.x = std::clamp(pos.x, minX, maxX - w);
}

}  // namespace si