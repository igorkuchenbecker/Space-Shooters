#pragma once

#include "scenes/Context.h"
#include "scenes/Scene.h"

namespace si {

class HighScoreScene final : public Scene {
public:
    explicit HighScoreScene(AppContext& ctx);

    void update(float dt) override;
    void draw() override;
    [[nodiscard]] std::string_view name() const override { return "HighScores"; }

private:
    AppContext& ctx_;
};

}  // namespace si