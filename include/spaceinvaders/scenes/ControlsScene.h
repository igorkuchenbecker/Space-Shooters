#pragma once

#include "scenes/Context.h"
#include "scenes/Scene.h"

namespace si {

class ControlsScene final : public Scene {
public:
    explicit ControlsScene(AppContext& ctx);

    void update(float dt) override;
    void draw() override;
    [[nodiscard]] std::string_view name() const override { return "Controls"; }

private:
    AppContext& ctx_;
};

}  // namespace si