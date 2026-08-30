#include "scenes/PlayingScene.h"

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

constexpr int kStarCount = 120;
constexpr float kClearedHoldSeconds = 2.0f;
constexpr float kGameOverHoldSeconds = 1.8f;
constexpr float kLevelIntroSeconds = 1.2f;
// O parallax acelera junto com a dificuldade (teto para não virar ruído).
constexpr float kStarSpeedPerLevel = 0.15f;
constexpr float kStarSpeedMax = 2.5f;

float starSpeedFor(int level) {
    const float speed = 1.0f + kStarSpeedPerLevel * static_cast<float>(level - 1);
    return speed > kStarSpeedMax ? kStarSpeedMax : speed;
}

void drawCentered(const char* text, int y, int size, ::Color color) {
    DrawText(text, cfg::kLogicalWidth / 2 - MeasureText(text, size) / 2, y, size, color);
}

// Mapeia um fato de gameplay para o efeito sonoro correspondente.
Sfx sfxFor(GameEvent event) {
    switch (event) {
        case GameEvent::PlayerShot:
            return Sfx::Shot;
        case GameEvent::EnemyShot:
            return Sfx::EnemyShot;
        case GameEvent::EnemyKilled:
            return Sfx::Explosion;
        case GameEvent::ShieldChipped:
            return Sfx::ShieldHit;
        case GameEvent::PlayerHit:
            return Sfx::PlayerHit;
        case GameEvent::ExtraLife:
            return Sfx::LevelClear;
        case GameEvent::LevelCleared:
            return Sfx::LevelClear;
        case GameEvent::GameOver:
            return Sfx::GameOver;
    }
    return Sfx::UiSelect;
}

}  // namespace

PlayingScene::PlayingScene(AppContext& ctx)
    : ctx_(ctx),
      session_(std::random_device{}()),
      background_(kStarCount, static_cast<std::uint64_t>(std::random_device{}())) {
    session_.reset();
    background_.setSpeed(starSpeedFor(session_.level()));
    // O HIGH do HUD acompanha a partida ao vivo a partir do recorde salvo.
    if (ctx_.highscores) {
        session_.setHighScore(ctx_.highscores->best());
    }
}

void PlayingScene::onExit() {
    if (ctx_.audio) {
        ctx_.audio->stopMusic();
    }
}

GameInput PlayingScene::readInput() const {
    GameInput out;
    out.left = IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A);
    out.right = IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D);
    out.fire = IsKeyDown(KEY_SPACE);

    if (IsGamepadAvailable(0)) {
        const float axis = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X);
        out.left = out.left || axis < -0.3f || IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT);
        out.right = out.right || axis > 0.3f || IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT);
        out.fire = out.fire || IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN);
    }
    return out;
}

void PlayingScene::enterPlaying() {
    phase_ = Phase::Playing;
    phaseTimer_ = 0.0f;
    sim_.reset();
    if (!musicStarted_ && ctx_.audio) {
        ctx_.audio->playMusic();
        musicStarted_ = true;
    }
}

void PlayingScene::playEventSounds() {
    if (ctx_.audio) {
        for (const GameEvent event : session_.events()) {
            ctx_.audio->play(sfxFor(event));
        }
    }
    session_.clearEvents();
}

// A simulação roda em passos fixos de 1/60 s: mesmo comportamento em 60, 144
// ou 30 FPS. A entrada é amostrada uma vez por frame e vale para os passos
// daquele frame.
void PlayingScene::stepSimulation(float dt) {
    const GameInput input = readInput();
    const int steps = sim_.push(dt);
    for (int i = 0; i < steps && session_.status() == GameSession::Status::Playing; ++i) {
        session_.update(cfg::kFixedDt, input);
    }
    playEventSounds();
}

void PlayingScene::onCleared() {
    // O bônus por nível já foi aplicado dentro da sessão ao concluir.
    phase_ = Phase::Cleared;
    phaseTimer_ = kClearedHoldSeconds;
}

void PlayingScene::onGameOver() {
    ctx_.lastScore = session_.score();
    ctx_.lastLevel = session_.level();
    ctx_.lastWon = false;
    if (ctx_.highscores) {
        ctx_.highscores->add(session_.score());
        ctx_.highscores->save(ctx_.highScoresPath);
    }
    phase_ = Phase::GameOverDelay;
    phaseTimer_ = kGameOverHoldSeconds;
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
            if (ui::backPressed()) {
                if (ctx_.audio) {
                    ctx_.audio->play(Sfx::UiSelect);
                }
                ctx_.scenes->push(std::make_unique<PauseScene>(ctx_));
                return;
            }

            stepSimulation(dt);

            switch (session_.status()) {
                case GameSession::Status::Won:
                    onCleared();
                    break;
                case GameSession::Status::Lost:
                    onGameOver();
                    break;
                case GameSession::Status::Playing:
                    break;
            }
            return;
        }
        case Phase::Cleared: {
            phaseTimer_ -= dt;
            if (phaseTimer_ <= 0.0f) {
                session_.startLevel(session_.level() + 1);
                session_.clearEvents();
                background_.setSpeed(starSpeedFor(session_.level()));
                phase_ = Phase::Intro;
                phaseTimer_ = kLevelIntroSeconds;
            }
            return;
        }
        case Phase::GameOverDelay: {
            phaseTimer_ -= dt;
            if (phaseTimer_ <= 0.0f) {
                ctx_.scenes->switchTo(std::make_unique<GameOverScene>(ctx_));
            }
            return;
        }
    }
}

void PlayingScene::draw() {
    background_.draw();
    drawSession(session_, time_);

    switch (phase_) {
        case Phase::Intro: {
            char buffer[32];
            std::snprintf(buffer, sizeof buffer, "LEVEL %d", session_.level());
            drawCentered(buffer, 240, 56, toRay(palette().accent));
            drawCentered("get ready!", 300, 24, toRay(palette().textDim));
            break;
        }
        case Phase::Cleared: {
            char buffer[48];
            std::snprintf(buffer, sizeof buffer, "LEVEL %d CLEARED  +%d BONUS", session_.level(),
                          session_.level() * cfg::kScoreBonusLevelClear);
            drawCentered(buffer, 240, 34, toRay(palette().text));
            break;
        }
        case Phase::GameOverDelay:
            drawCentered("GAME OVER", 250, 52, toRay(rgb(255, 90, 90)));
            break;
        case Phase::Playing:
            break;
    }

    drawHud(session_.score(), session_.highScore(), session_.lives(), session_.level());
}

}  // namespace si
