#include "core/Rng.h"

namespace si {

namespace {

constexpr std::uint64_t kMul = 0x2545F4914F6CDD1Dull;

}  // namespace

Rng::Rng(std::uint64_t seed) { this->seed(seed); }

void Rng::seed(std::uint64_t value) {
    // Evita estado zero, que deixaria o xorshift preso em 0.
    state_ = value == 0 ? 0x9E3779B97F4A7C15ull : value;
}

std::uint32_t Rng::next() {
    state_ ^= state_ >> 12;
    state_ ^= state_ << 25;
    state_ ^= state_ >> 27;
    return static_cast<std::uint32_t>((state_ * kMul) >> 32);
}

float Rng::nextFloat() {
    // 24 bits de aleatoriedade já bastam para ranges de gameplay.
    return static_cast<float>(next() & 0xFFFFFFu) * (1.0f / 16777216.0f);
}

float Rng::range(float lo, float hi) { return lo + (hi - lo) * nextFloat(); }

int Rng::intRange(int lo, int hiExclusive) {
    if (hiExclusive <= lo) {
        return lo;
    }
    return lo + static_cast<int>(next() % static_cast<std::uint32_t>(hiExclusive - lo));
}

bool Rng::chance(float probability01) {
    if (probability01 <= 0.0f) {
        return false;
    }
    if (probability01 >= 1.0f) {
        return true;
    }
    return nextFloat() < probability01;
}

}  // namespace si