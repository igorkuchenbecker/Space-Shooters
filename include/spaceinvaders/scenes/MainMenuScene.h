#pragma once

#include "scenes/Context.h"
#include "scenes/Scene.h"

namespace si {

class MainMenuScene final : public Scene {
public:
    explicit MainMenuScene(AppContext& ctx);

    void update(float dt) override;
    void draw() override;
    [[nodiscard]] std::string_view name() const override { return "MainMenu"; }

private:
    AppContext& ctx_;
    int selection_ = 0;
    float titlePulse_ = 0.0f;

    void moveSelection(int delta);
    void activate();
};

}  // namespace si