#include "entities/Particle.h"

#include <algorithm>
#include <cmath>

namespace si {

namespace {

constexpr float kGolden = 2.399963f;  // ângulo pseudo-aleatório determinístico

}

void ParticleSystem::emitBurst(Vec2 pos, Color color, int count, float speed, float life) {
    // Uma única alocação na primeira emissão; depois o vetor nunca cresce.
    if (particles_.capacity() < kMax) {
        particles_.reserve(kMax);
    }
    for (int i = 0; i < count; ++i) {
        if (particles_.size() >= kMax) {
            return;
        }
        const float angle = static_cast<float>(i) * kGolden;
        const float s = speed * (0.35f + std::fmod(static_cast<float>(i) * 0.6180339887f, 0.65f));
        particles_.push_back(Particle{pos,
                                      Vec2{std::cos(angle) * s, std::sin(angle) * s},
                                      i % 2 == 0 ? 3.0f : 2.0f,
                                      0.0f,
                                      life,
                                      color,
                                      false});
    }
}

void ParticleSystem::update(float dt) {
    const float damp = 1.0f - 3.0f * dt;
    for (auto& p : particles_) {
        if (p.dead) {
            continue;
        }
        p.age += dt;
        if (p.age >= p.life) {
            p.dead = true;
            continue;
        }
        p.pos += p.vel * dt;
        p.vel = p.vel * (damp > 0.0f ? damp : 0.0f);
    }
    particles_.erase(std::remove_if(particles_.begin(), particles_.end(), [](const Particle& p) { return p.dead; }),
                     particles_.end());
}

void ParticleSystem::clear() { particles_.clear(); }

}  // namespace si