#include "scenes/PauseScene.h"

#include <array>

#include "raylib.h"

#include "core/Config.h"
#include "rendering/Convert.h"
#include "scenes/MainMenuScene.h"
#include "scenes/SceneManager.h"
#include "scenes/UiInput.h"

namespace si {

namespace {

constexpr std::array<const char*, 2> kOptions = {"RESUME", "MAIN MENU"};

void drawCentered(const char* text, int y, int size, ::Color color) {
    DrawText(text, cfg::kLogicalWidth / 2 - MeasureText(text, size) / 2, y, size, color);
}

}  // namespace

PauseScene::PauseScene(AppContext& ctx) : ctx_(ctx) {}

void PauseScene::onEnter() {
    if (ctx_.audio) {
        ctx_.audio->pauseMusic();
    }
    selection_ = 0;
    resumeMusicOnExit_ = true;
}

void PauseScene::onExit() {
    if (resumeMusicOnExit_ && ctx_.audio) {
        ctx_.audio->resumeMusic();
    }
}

void PauseScene::moveSelection(int index) {
    if (index == selection_) {
        return;
    }
    selection_ = index;
    if (ctx_.audio) {
        ctx_.audio->play(Sfx::UiSelect);
    }
}

void PauseScene::confirm() {
    if (ctx_.audio) {
        ctx_.audio->play(Sfx::UiSelect);
    }
    if (selection_ == 0) {
        ctx_.scenes->pop();
        return;
    }
    resumeMusicOnExit_ = false;
    ctx_.scenes->switchTo(std::make_unique<MainMenuScene>(ctx_));
}

void PauseScene::update(float) {
    if (ui::upPressed() ||
        (IsGamepadAvailable(0) && GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y) < -0.5f)) {
        moveSelection(0);
    }
    if (ui::downPressed() ||
        (IsGamepadAvailable(0) && GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y) > 0.5f)) {
        moveSelection(1);
    }
    if (ui::backPressed()) {
        if (ctx_.audio) {
            ctx_.audio->play(Sfx::UiSelect);
        }
        ctx_.scenes->pop();
        return;
    }
    if (ui::confirmPressed()) {
        confirm();
    }
}

void PauseScene::draw() {
    // Escurece o jogo congelado desenhado atrás (a cena de gameplay não foi
    // atualizada, então a imagem é estável).
    DrawRectangle(0, 0, cfg::kLogicalWidth, cfg::kLogicalHeight, {0, 0, 0, 150});

    drawCentered("PAUSED", 190, 72, toRay(palette().accent));

    constexpr int kFirstY = 320;
    for (std::size_t i = 0; i < kOptions.size(); ++i) {
        const int y = kFirstY + static_cast<int>(i) * 44;
        const bool selected = static_cast<int>(i) == selection_;
        const ::Color c = selected ? toRay(palette().accent) : toRay(palette().textDim);
        if (selected) {
            DrawText(">", cfg::kLogicalWidth / 2 - 120, y, 28, c);
            drawCentered(kOptions[i], y, 28, c);
            DrawText("<", cfg::kLogicalWidth / 2 + 108, y, 28, c);
        } else {
            drawCentered(kOptions[i], y, 28, c);
        }
    }
    drawCentered("esc resumes", 480, 16, toRay(palette().textDim));
}

}  // namespace si