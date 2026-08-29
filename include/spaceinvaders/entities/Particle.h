#pragma once

#include <vector>

#include "core/Color.h"
#include "core/Vec2.h"

namespace si {

struct Particle {
    Vec2 pos{};
    Vec2 vel{};
    float size = 2.0f;
    float age = 0.0f;
    float life = 0.5f;
    Color color{};
    bool dead = false;
};

// Sistema simples de partículas: explosões e feedback visual. Tamanho
// limitado, sem alocações por frame.
class ParticleSystem {
public:
    static constexpr std::size_t kMax = 512;

    void emitBurst(Vec2 pos, Color color, int count, float speed, float life);

    void update(float dt);

    void clear();

    [[nodiscard]] const std::vector<Particle>& particles() const { return particles_; }

private:
    std::vector<Particle> particles_;
};

}  // namespace si