#pragma once

#include <memory>
#include <optional>
#include <vector>

#include "scenes/Scene.h"

namespace si {

// Pilha de cenas. Só o topo atualiza; todas desenham (de baixo para cima),
// permitindo sobreposição (ex: pause sobre o gameplay). Troca de cena com
// fade-out/fade-in via overlay preto.
class SceneManager {
public:
    explicit SceneManager(int logicalWidth, int logicalHeight, float fadeDuration = 0.22f);

    // Substitui a pilha inteira (primeira cena, sem fade).
    void init(std::unique_ptr<Scene> first);

    // Troca para outra cena com transição (esvazia a pilha).
    void switchTo(std::unique_ptr<Scene> next);

    // Empilha por cima da atual (sem fade — usado no pause).
    void push(std::unique_ptr<Scene> next);

    void pop();

    void update(float dt);
    void draw();

    [[nodiscard]] bool empty() const { return stack_.empty() && !transition_; }
    [[nodiscard]] const Scene* top() const;

private:
    struct Transition {
        std::unique_ptr<Scene> next;
        float elapsed = 0.0f;
        int phase = 0;  // 0 = fade-out da atual, 1 = fade-in da nova
    };

    float fadeDuration_;
    int logicalWidth_;
    int logicalHeight_;
    std::vector<std::unique_ptr<Scene>> stack_;
    std::optional<Transition> transition_;

    void commitSwitch();
    void advance(float dt);
};

}  // namespace si