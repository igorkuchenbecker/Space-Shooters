#pragma once

#include <cstdint>

namespace si {

struct Color {
    std::uint8_t r = 0;
    std::uint8_t g = 0;
    std::uint8_t b = 0;
    std::uint8_t a = 255;
};

inline constexpr Color rgb(std::uint8_t r, std::uint8_t g, std::uint8_t b) { return {r, g, b, 255}; }
inline constexpr Color rgba(std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a) {
    return {r, g, b, a};
}

}  // namespace si