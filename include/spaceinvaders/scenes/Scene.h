#pragma once

#include <string_view>

namespace si {

// Interface base de cena. `draw()` é chamado já dentro do modo de textura da
// resolução lógica. `update()` recebe o dt fixo da simulação.
class Scene {
public:
    virtual ~Scene() = default;

    virtual void onEnter() {}
    virtual void onExit() {}

    virtual void update(float dt) = 0;
    virtual void draw() = 0;

    [[nodiscard]] virtual std::string_view name() const = 0;
};

}  // namespace si