#include "rendering/Starfield.h"

#include "raylib.h"

#include "rendering/Convert.h"

namespace si {

Starfield::Starfield(int count, std::uint64_t seed) {
    Rng rng(seed);
    stars_.reserve(static_cast<std::size_t>(count));
    for (int i = 0; i < count; ++i) {
        const float layer = rng.nextFloat();  // 0 = fundo (lento) ... 1 = frente
        Star s;
        s.x = rng.range(0.0f, static_cast<float>(cfg::kLogicalWidth));
        s.y = rng.range(0.0f, static_cast<float>(cfg::kLogicalHeight));
        s.size = 1.0f + layer * 2.0f;
        s.speed = 8.0f + layer * 30.0f;
        s.alpha = static_cast<unsigned char>(60 + static_cast<int>(layer * 195));
        stars_.push_back(s);
    }
}

void Starfield::update(float dt) {
    for (auto& s : stars_) {
        s.y += s.speed * speed_ * dt;
        if (s.y > static_cast<float>(cfg::kLogicalHeight)) {
            s.y = 0.0f;
            s.x += 7.0f;
            if (s.x > static_cast<float>(cfg::kLogicalWidth)) {
                s.x = 0.0f;
            }
        }
    }
}

void Starfield::draw() const {
    for (const auto& s : stars_) {
        DrawCircleV({s.x, s.y}, s.size, {200, 210, 255, s.alpha});
    }
}

}  // namespace si