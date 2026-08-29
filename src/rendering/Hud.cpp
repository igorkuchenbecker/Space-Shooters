#include "rendering/Hud.h"

#include <cstdio>

#include "raylib.h"

#include "core/Config.h"
#include "rendering/Convert.h"

namespace si {

void drawHud(std::int64_t score, std::int64_t highScore, int lives, int level) {
    const int size = 20;

    const ::Color text = toRay(palette().text);
    const ::Color dim = toRay(palette().textDim);

    DrawLine(0, 28, cfg::kLogicalWidth, 28, toRay(palette().border));

    char buffer[64];
    std::snprintf(buffer, sizeof buffer, "SCORE  %06lld", static_cast<long long>(score));
    DrawText(buffer, 12, 6, size, text);

    std::snprintf(buffer, sizeof buffer, "HIGH  %06lld", static_cast<long long>(highScore));
    DrawText(buffer, cfg::kLogicalWidth / 2 - MeasureText(buffer, size) / 2, 6, size, text);

    std::snprintf(buffer, sizeof buffer, "LIVES  %d", lives);
    DrawText(buffer, cfg::kLogicalWidth - 260, 6, size, text);

    std::snprintf(buffer, sizeof buffer, "LEVEL  %d", level);
    DrawText(buffer, cfg::kLogicalWidth - 96, 6, size, dim);
}

}  // namespace si