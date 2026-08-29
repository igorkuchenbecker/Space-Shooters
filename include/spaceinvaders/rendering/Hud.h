#pragma once

#include <cstdint>

namespace si {

// Barra superior de informações: SCORE / HIGH SCORE / LIVES / LEVEL.
void drawHud(std::int64_t score, std::int64_t highScore, int lives, int level);

}  // namespace si