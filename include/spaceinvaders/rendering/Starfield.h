#pragma once

#include <vector>

#include "core/Config.h"
#include "core/Rng.h"

namespace si {

// Fundo em camadas parallax (sem assets): pontos que descem e voltam ao topo.
class Starfield {
public:
    Starfield(int count, std::uint64_t seed);

    void update(float dt);
    void draw() const;

    void setSpeed(float multiplier) { speed_ = multiplier; }

private:
    struct Star {
        float x;
        float y;
        float size;
        float speed;
        unsigned char alpha;
    };

    std::vector<Star> stars_;
    float speed_ = 1.0f;
};

}  // namespace si