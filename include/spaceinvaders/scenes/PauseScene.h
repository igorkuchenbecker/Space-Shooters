#pragma once

#include "scenes/Context.h"
#include "scenes/Scene.h"

namespace si {

// Sobreposição de pausa: congela o gameplay (fica no topo da pilha de cenas),
// mostra overlay e permite continuar ou voltar ao menu.
class PauseScene final : public Scene {
public:
    explicit PauseScene(AppContext& ctx);

    void onEnter() override;
    void onExit() override;

    void update(float dt) override;
    void draw() override;
    [[nodiscard]] std::string_view name() const override { return "Pause"; }

private:
    AppContext& ctx_;
    int selection_ = 0;
    // Sair do pause para o menu não deve retomar a música da partida.
    bool resumeMusicOnExit_ = true;

    void moveSelection(int index);
    void confirm();
};

}  // namespace si