#pragma once

namespace si {

// Timestep fixo com acumulador. Recebe o dt do frame e devolve quantas
// atualizações de simulação devem rodar, mantendo o gameplay consistente
// entre taxas de FPS e sem "espiral da morte" sob dt gigantes.
class FixedStep {
public:
    explicit FixedStep(float stepSeconds);

    // dt do frame em segundos. Retorna o número de passos fixos a executar
    // neste frame (0 se o acumulador ainda não chegou em um passo cheio).
    [[nodiscard]] int push(float frameDt);

    void reset();

private:
    float step_;
    float accumulator_ = 0.0f;
};

}  // namespace si