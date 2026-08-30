#include "rendering/Hud.h"

#include <cstdio>

#include "raylib.h"

#include "core/Config.h"
#include "rendering/Convert.h"

namespace si {

namespace {

constexpr int kFontSize = 20;
constexpr int kBaselineY = 6;
constexpr int kMargin = 12;
constexpr int kSeparatorY = 28;

void drawRightAligned(const char* text, int right, int y, int size, ::Color color) {
    DrawText(text, right - MeasureText(text, size), y, size, color);
}

}  // namespace

void drawHud(std::int64_t score, std::int64_t highScore, int lives, int level) {
    const ::Color text = toRay(palette().text);
    const ::Color dim = toRay(palette().textDim);

    DrawLine(0, kSeparatorY, cfg::kLogicalWidth, kSeparatorY, toRay(palette().border));

    char buffer[64];
    std::snprintf(buffer, sizeof buffer, "SCORE  %06lld", static_cast<long long>(score));
    DrawText(buffer, kMargin, kBaselineY, kFontSize, text);

    std::snprintf(buffer, sizeof buffer, "HIGH  %06lld", static_cast<long long>(highScore));
    DrawText(buffer, cfg::kLogicalWidth / 2 - MeasureText(buffer, kFontSize) / 2, kBaselineY, kFontSize, text);

    // LEVEL encosta na direita e LIVES se acomoda à esquerda dele: a barra
    // continua legível quando os números crescem.
    std::snprintf(buffer, sizeof buffer, "LEVEL  %d", level);
    const int levelWidth = MeasureText(buffer, kFontSize);
    drawRightAligned(buffer, cfg::kLogicalWidth - kMargin, kBaselineY, kFontSize, dim);

    std::snprintf(buffer, sizeof buffer, "LIVES  %d", lives);
    drawRightAligned(buffer, cfg::kLogicalWidth - kMargin - levelWidth - 24, kBaselineY, kFontSize, text);
}

}  // namespace si
