#pragma once

#include "scenes/Context.h"
#include "scenes/Scene.h"

namespace si {

class GameOverScene final : public Scene {
public:
    explicit GameOverScene(AppContext& ctx);

    void update(float dt) override;
    void draw() override;
    [[nodiscard]] std::string_view name() const override { return "GameOver"; }

private:
    AppContext& ctx_;
    int selection_ = 0;
    bool newRecord_ = false;
};

}  // namespace si