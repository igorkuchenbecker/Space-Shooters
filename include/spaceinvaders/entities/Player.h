#pragma once

#include "core/Config.h"
#include "core/Rect.h"
#include "core/Vec2.h"

namespace si {

struct Player {
    Vec2 pos{};            // top-left
    float w = cfg::kPlayerWidth;
    float h = cfg::kPlayerHeight;
    float fireCooldown = 0.0f;
    float invulnTimer = 0.0f;
    int lives = 0;
    bool moving = false;
    float minX = 0.0f;
    float maxX = 0.0f;

    void reset(Vec2 spawnPos, int startLives, float rangeMinX, float rangeMaxX);

    void update(float dt, bool left, bool right);

    [[nodiscard]] bool canFire() const { return fireCooldown <= 0.0f; }
    void beginFireCooldown() { fireCooldown = cfg::kPlayerFireCooldown; }

    void hit() { invulnTimer = cfg::kPlayerInvulnTime; }
    [[nodiscard]] bool isInvulnerable() const { return invulnTimer > 0.0f; }

    [[nodiscard]] Rect rect() const { return makeRect(pos, Vec2{w, h}); }
};

}  // namespace si