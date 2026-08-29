#pragma once

#include <cstdint>

namespace si {

// RNG determinístico (xorshift64*) — jogabilidade e testes reproduzíveis
// com a mesma seed. Não usa <random> global para não depender do estado
// estático da libc.
class Rng {
public:
    explicit Rng(std::uint64_t seed = 0x9E3779B97F4A7C15ull);

    void seed(std::uint64_t value);

    [[nodiscard]] std::uint32_t next();

    // Float no intervalo [0, 1).
    [[nodiscard]] float nextFloat();

    [[nodiscard]] float range(float lo, float hi);

    // Inteiro no intervalo [lo, hiExclusive).
    [[nodiscard]] int intRange(int lo, int hiExclusive);

    [[nodiscard]] bool chance(float probability01);

private:
    std::uint64_t state_;
};

}  // namespace si