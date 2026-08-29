#include "scenes/MainMenuScene.h"

#include <array>

#include <cmath>

#include "raylib.h"

#include "core/Config.h"
#include "rendering/Convert.h"
#include "rendering/DrawGame.h"
#include "scenes/ControlsScene.h"
#include "scenes/GameOverScene.h"
#include "scenes/HighScoreScene.h"
#include "scenes/PlayingScene.h"
#include "scenes/SceneManager.h"
#include "scenes/UiInput.h"

namespace si {

namespace {

constexpr std::array<const char*, 4> kOptions = {"PLAY", "HIGH SCORES", "CONTROLS", "EXIT"};

void drawCentered(const char* text, int y, int size, ::Color color) {
    DrawText(text, cfg::kLogicalWidth / 2 - MeasureText(text, size) / 2, y, size, color);
}

}  // namespace

MainMenuScene::MainMenuScene(AppContext& ctx) : ctx_(ctx) {}

void MainMenuScene::moveSelection(int delta) {
    selection_ = (selection_ + delta + static_cast<int>(kOptions.size())) % static_cast<int>(kOptions.size());
    if (ctx_.audio) {
        ctx_.audio->play(Sfx::UiSelect);
    }
}

void MainMenuScene::activate() {
    switch (selection_) {
        case 0:  // PLAY
            ctx_.scenes->switchTo(std::make_unique<PlayingScene>(ctx_));
            break;
        case 1:  // HIGH SCORES
            ctx_.scenes->switchTo(std::make_unique<HighScoreScene>(ctx_));
            break;
        case 2:  // CONTROLS
            ctx_.scenes->switchTo(std::make_unique<ControlsScene>(ctx_));
            break;
        case 3:  // EXIT
            ctx_.quitRequested = true;
            break;
        default:
            break;
    }
}

void MainMenuScene::update(float dt) {
    titlePulse_ += dt;

    if (ui::upPressed() ||
        (IsGamepadAvailable(0) && GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y) < -0.5f)) {
        moveSelection(-1);
    }
    if (ui::downPressed() ||
        (IsGamepadAvailable(0) && GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y) > 0.5f)) {
        moveSelection(1);
    }
    if (ui::confirmPressed()) {
        activate();
    }
}

void MainMenuScene::draw() {
    const auto& pal = palette();

    const float pulse = (std::sin(titlePulse_ * 2.4f) + 1.0f) * 0.5f;
    ::Color titleColor = toRay(pal.accent);
    titleColor.g = static_cast<unsigned char>(205.0f + pulse * 50.0f);

    drawCentered("SPACE", 150, 64, toRay(pal.text));
    drawCentered("INVADERS", 210, 84, titleColor);

    constexpr int kFirstY = 340;
    constexpr int kSpacing = 44;
    for (std::size_t i = 0; i < kOptions.size(); ++i) {
        const int y = kFirstY + static_cast<int>(i) * kSpacing;
        const bool selected = static_cast<int>(i) == selection_;
        const ::Color c = selected ? toRay(pal.accent) : toRay(pal.textDim);
        if (selected) {
            DrawText(">", cfg::kLogicalWidth / 2 - 120, y, 28, c);
            DrawText("<", cfg::kLogicalWidth / 2 + 108, y, 28, c);
            drawCentered(kOptions[i], y, 28, c);
        } else {
            drawCentered(kOptions[i], y, 28, c);
        }
    }

    drawCentered("ARROWS / WASD + ENTER", 505, 14, toRay(pal.textDim));
}

}  // namespace si