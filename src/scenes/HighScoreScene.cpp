#include "scenes/HighScoreScene.h"

#include <cstdio>
#include <vector>

#include "raylib.h"

#include "core/Config.h"
#include "rendering/Convert.h"
#include "scenes/MainMenuScene.h"
#include "scenes/SceneManager.h"
#include "scenes/UiInput.h"

namespace si {

namespace {

void drawCentered(const char* text, int y, int size, ::Color color) {
    DrawText(text, cfg::kLogicalWidth / 2 - MeasureText(text, size) / 2, y, size, color);
}

}  // namespace

HighScoreScene::HighScoreScene(AppContext& ctx) : ctx_(ctx) {}

void HighScoreScene::update(float) {
    if (ui::backPressed() || ui::confirmPressed()) {
        if (ctx_.audio) {
            ctx_.audio->play(Sfx::UiSelect);
        }
        ctx_.scenes->switchTo(std::make_unique<MainMenuScene>(ctx_));
    }
}

void HighScoreScene::draw() {
    drawCentered("HIGH SCORES", 84, 52, toRay(palette().accent));

    // Nada de copiar o vetor por frame: o ternário com um prvalue faria isso.
    static const std::vector<std::int64_t> kEmpty;
    const std::vector<std::int64_t>& list = ctx_.highscores ? ctx_.highscores->list() : kEmpty;
    if (list.empty()) {
        drawCentered("no records yet — good luck!", 240, 24, toRay(palette().textDim));
    } else {
        int y = 190;
        int rank = 1;
        for (const auto score : list) {
            char line[32];
            std::snprintf(line, sizeof line, "%d.  %06lld", rank++, static_cast<long long>(score));
            drawCentered(line, y, 30, toRay(palette().text));
            y += 40;
        }
    }

    drawCentered("enter / esc to return", 480, 18, toRay(palette().textDim));
}

}  // namespace si