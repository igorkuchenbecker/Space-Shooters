#include "scenes/ControlsScene.h"

#include <array>

#include "raylib.h"

#include "core/Config.h"
#include "rendering/Convert.h"
#include "scenes/MainMenuScene.h"
#include "scenes/SceneManager.h"
#include "scenes/UiInput.h"

namespace si {

namespace {

struct ControlRow {
    const char* keys;
    const char* action;
};

constexpr std::array<ControlRow, 7> kControls = {{
    {"A / D  or  LEFT / RIGHT", "move spacecraft"},
    {"SPACE", "fire"},
    {"ESC", "pause / menu"},
    {"ENTER", "confirm"},
    {"GAMEPAD", "dpad + A fire + ESC (back)"},
    {"", ""},
    {"GOAL:", "clear the invasion, earn the highest score"},
}};

void drawCentered(const char* text, int y, int size, ::Color color) {
    DrawText(text, cfg::kLogicalWidth / 2 - MeasureText(text, size) / 2, y, size, color);
}

}  // namespace

ControlsScene::ControlsScene(AppContext& ctx) : ctx_(ctx) {}

void ControlsScene::update(float) {
    if (ui::backPressed() || ui::confirmPressed()) {
        if (ctx_.audio) {
            ctx_.audio->play(Sfx::UiSelect);
        }
        ctx_.scenes->switchTo(std::make_unique<MainMenuScene>(ctx_));
    }
}

void ControlsScene::draw() {
    drawCentered("CONTROLS", 84, 52, toRay(palette().accent));

    // `const char*` direto: DrawText por frame não deve alocar nada.
    int y = 170;
    for (const auto& row : kControls) {
        if (row.keys[0] == '\0') {
            continue;
        }
        DrawText(row.keys, cfg::kLogicalWidth / 2 - 240, y, 22, toRay(palette().text));
        DrawText(row.action, cfg::kLogicalWidth / 2 + 40, y, 22, toRay(palette().textDim));
        y += 38;
    }

    drawCentered("enter / esc to return", 480, 18, toRay(palette().textDim));
}

}  // namespace si