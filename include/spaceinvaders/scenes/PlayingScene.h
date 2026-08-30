#pragma once

#include "core/FixedStep.h"
#include "core/GameSession.h"
#include "rendering/Starfield.h"
#include "scenes/Context.h"
#include "scenes/Scene.h"

namespace si {

class PlayingScene final : public Scene {
public:
    explicit PlayingScene(AppContext& ctx);

    void update(float dt) override;
    void draw() override;
    void onExit() override;
    [[nodiscard]] std::string_view name() const override { return "Playing"; }

private:
    enum class Phase { Intro, Playing, Cleared, GameOverDelay };

    AppContext& ctx_;
    GameSession session_;
    Starfield background_;
    FixedStep sim_{cfg::kFixedDt};
    Phase phase_ = Phase::Intro;
    float phaseTimer_ = 1.0f;
    float time_ = 0.0f;
    bool musicStarted_ = false;

    [[nodiscard]] GameInput readInput() const;
    void enterPlaying();
    void stepSimulation(float dt);
    void playEventSounds();
    void onCleared();
    void onGameOver();
};

}  // namespace si
