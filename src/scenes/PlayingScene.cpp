#include "scenes/PlayingScene.h"

#include <array>
#include <cstdio>
#include <random>

#include "raylib.h"

#include "core/Config.h"
#include "rendering/Convert.h"
#include "rendering/DrawGame.h"
#include "rendering/Hud.h"
#include "scenes/GameOverScene.h"
#include "scenes/PauseScene.h"
#include "scenes/SceneManager.h"
#include "scenes/UiInput.h"

namespace si {

namespace {

void drawCentered(const char* text, int y, int size, ::Color color) {
    DrawText(text, cfg::kLogicalWidth / 2 - MeasureText(text, size) / 2, y, size, color);
}

}  // namespace

PlayingScene::PlayingScene(AppContext& ctx)
    : ctx_(ctx),
      session_(std::random_device{}()),
      background_(120, static_cast<std::uint64_t>(std::random_device{}())) {
    session_.reset();
}

void PlayingScene::readInput(GameInput& out) {
    if (ui::backPressed()) {
        escapedThisFrame_ = true;
        return;
    }

    out.left = IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A);
    out.right = IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D);
    out.fire = IsKeyDown(KEY_SPACE);

    if (IsGamepadAvailable(0)) {
        const float axis = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X);
        if (axis < -0.3f) {
            out.left = true;
        }
        if (axis > 0.3f) {
            out.right = true;
        }
        out.left = out.left || IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT);
        out.right = out.right || IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT);
        out.fire = out.fire || IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN);
    }
}

void PlayingScene::enterPlaying() {
    phase_ = Phase::Playing;
    phaseTimer_ = 0.0f;
    if (!musicStarted_ && ctx_.audio) {
        ctx_.audio->playMusic();
        musicStarted_ = true;
    }
}

void PlayingScene::onCleared() {
    if (ctx_.audio) {
        ctx_.audio->play(Sfx::LevelClear);
    }
    // O bônus por nível já foi aplicado dentro da sessão ao concluir.
    phase_ = Phase::Cleared;
    phaseTimer_ = 2.0f;
}

void PlayingScene::onGameOver() {
    if (ctx_.audio) {
        ctx_.audio->play(Sfx::GameOver);
    }
    ctx_.lastScore = session_.score();
    ctx_.lastLevel = session_.level();
    ctx_.lastWon = false;
    if (ctx_.highscores) {
        ctx_.highscores->add(session_.score());
        ctx_.highscores->save(defaultHighScoresPath());
    }
    phase_ = Phase::GameOverDelay;
    phaseTimer_ = 1.8f;
}

void PlayingScene::update(float dt) {
    time_ += dt;
    background_.update(dt);

    switch (phase_) {
        case Phase::Intro: {
            phaseTimer_ -= dt;
            if (phaseTimer_ <= 0.0f) {
                enterPlaying();
            }
            return;
        }
        case Phase::Playing: {
            escapedThisFrame_ = false;
            GameInput input;
            readInput(input);
            if (escapedThisFrame_) {
                if (ctx_.audio) {
                    ctx_.audio->play(Sfx::UiSelect);
                }
                ctx_.scenes->push(std::make_unique<PauseScene>(ctx_));
                return;
            }

            session_.update(dt, input);
            const auto after = session_.status();

            if (after == GameSession::Status::Won) {
                onCleared();
            } else if (after == GameSession::Status::Lost) {
                onGameOver();
            }
            return;
        }
        case Phase::Cleared: {
            phaseTimer_ -= dt;
            if (phaseTimer_ <= 0.0f) {
                session_.startLevel(session_.level() + 1);
                phase_ = Phase::Intro;
                phaseTimer_ = 1.2f;
            }
            return;
        }
        case Phase::GameOverDelay: {
            phaseTimer_ -= dt;
            if (phaseTimer_ <= 0.0f) {
                if (ctx_.audio) {
                    ctx_.audio->stopMusic();
                }
                ctx_.scenes->switchTo(std::make_unique<GameOverScene>(ctx_));
            }
            return;
        }
    }
}

void PlayingScene::draw() {
    background_.draw();

    switch (phase_) {
        case Phase::Intro:
            drawSession(session_, time_);
            {
                char buffer[32];
                std::snprintf(buffer, sizeof buffer, "LEVEL %d", session_.level());
                drawCentered(buffer, 240, 56, toRay(palette().accent));
            }
            drawCentered("get ready!", 300, 24, toRay(palette().textDim));
            break;
        case Phase::Playing:
        case Phase::Cleared:
            drawSession(session_, time_);
            if (phase_ == Phase::Cleared) {
                char buffer[48];
                std::snprintf(buffer, sizeof buffer, "LEVEL %d CLEARED  +%d BONUS", session_.level(),
                              session_.level() * cfg::kScoreBonusLevelClear);
                drawCentered(buffer, 240, 34, toRay(palette().text));
            }
            break;
        case Phase::GameOverDelay:
            drawSession(session_, time_);
            drawCentered("GAME OVER", 250, 52, toRay(rgb(255, 90, 90)));
            break;
        default:
            break;
    }

    const std::int64_t high = ctx_.highscores ? ctx_.highscores->best() : 0;
    drawHud(session_.score(), high, session_.lives(), session_.level());
}

}  // namespace si