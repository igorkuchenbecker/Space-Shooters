#pragma once

#include <cstdint>

#include "core/Vec2.h"

namespace si {

// Dono do projétil — impossível confundir tiro do jogador com tiro inimigo.
enum class Owner : std::uint8_t { Player, Enemy };

struct Projectile {
    Owner owner = Owner::Player;
    Vec2 pos{};          // top-left
    Vec2 vel{};
    float w = 0.0f;
    float h = 0.0f;
    float life = 1e9f;   // segundos restantes (só tiros inimigos expiram)
    bool dead = false;
};

}  // namespace si