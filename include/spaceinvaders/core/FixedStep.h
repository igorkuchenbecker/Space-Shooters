#pragma once

namespace si {

// Timestep fixo com acumulador. Recebe o dt do frame e devolve quantas
// atualizações de simulação devem rodar, mantendo o gameplay consistente
// entre taxas de FPS e sem "espiral da morte" sob dt gigantes.
class FixedStep {
public:
    // dt acumulável por frame; acima disso o excedente é descartado (o jogo
    // desacelera em vez de tentar recuperar o tempo perdido para sempre).
    static constexpr float kMaxFrameDt = 0.25f;

    explicit FixedStep(float stepSeconds);

    // dt do frame em segundos. Retorna o número de passos fixos a executar
    // neste frame (0 se o acumulador ainda não chegou em um passo cheio).
    // dt negativo ou NaN é ignorado; um `stepSeconds` inválido rende 0 passos.
    [[nodiscard]] int push(float frameDt);

    void reset();

    [[nodiscard]] float step() const { return step_; }

    // Fração do passo já acumulada em [0, 1): útil para interpolar o render.
    [[nodiscard]] float alpha() const;

private:
    float step_;
    float accumulator_ = 0.0f;
};

}  // namespace si
