#include "scenes/GameOverScene.h"

#include <array>
#include <cstdio>

#include "raylib.h"

#include "core/Config.h"
#include "rendering/Convert.h"
#include "scenes/MainMenuScene.h"
#include "scenes/PlayingScene.h"
#include "scenes/SceneManager.h"
#include "scenes/UiInput.h"

namespace si {

namespace {

constexpr std::array<const char*, 2> kOptions = {"PLAY AGAIN", "MAIN MENU"};

void drawCentered(const char* text, int y, int size, ::Color color) {
    DrawText(text, cfg::kLogicalWidth / 2 - MeasureText(text, size) / 2, y, size, color);
}

}  // namespace

GameOverScene::GameOverScene(AppContext& ctx) : ctx_(ctx) {
    newRecord_ = ctx_.highscores && ctx_.highscores->qualifies(ctx_.lastScore);
}

void GameOverScene::moveSelection(int index) {
    if (index == selection_) {
        return;  // sem blip repetido quando já está na opção
    }
    selection_ = index;
    if (ctx_.audio) {
        ctx_.audio->play(Sfx::UiSelect);
    }
}

void GameOverScene::update(float) {
    if (ui::upPressed() ||
        (IsGamepadAvailable(0) && GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y) < -0.5f)) {
        moveSelection(0);
    }
    if (ui::downPressed() ||
        (IsGamepadAvailable(0) && GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y) > 0.5f)) {
        moveSelection(1);
    }
    if (ui::confirmPressed()) {
        if (ctx_.audio) {
            ctx_.audio->play(Sfx::UiSelect);
        }
        if (selection_ == 0) {
            ctx_.scenes->switchTo(std::make_unique<PlayingScene>(ctx_));
        } else {
            ctx_.scenes->switchTo(std::make_unique<MainMenuScene>(ctx_));
        }
    }
}

void GameOverScene::draw() {
    drawCentered("GAME OVER", 110, 72, toRay(rgb(255, 90, 90)));

    char buffer[64];
    std::snprintf(buffer, sizeof buffer, "SCORE  %06lld", static_cast<long long>(ctx_.lastScore));
    drawCentered(buffer, 230, 30, toRay(palette().text));

    std::snprintf(buffer, sizeof buffer, "LEVEL REACHED  %d", ctx_.lastLevel);
    drawCentered(buffer, 276, 22, toRay(palette().textDim));

    if (newRecord_) {
        drawCentered("*** NEW HIGH SCORE ***", 320, 26, toRay(palette().accent));
    }

    constexpr int kFirstY = 380;
    for (std::size_t i = 0; i < kOptions.size(); ++i) {
        const int y = kFirstY + static_cast<int>(i) * 44;
        const bool selected = static_cast<int>(i) == selection_;
        const ::Color c = selected ? toRay(palette().accent) : toRay(palette().textDim);
        if (selected) {
            DrawText(">", cfg::kLogicalWidth / 2 - 120, y, 28, c);
        }
        drawCentered(kOptions[i], y, 28, c);
        if (selected) {
            DrawText("<", cfg::kLogicalWidth / 2 + 108, y, 28, c);
        }
    }
}

}  // namespace si