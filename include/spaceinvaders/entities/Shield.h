#pragma once

#include <array>
#include <cstdint>

#include "core/Config.h"
#include "core/Rect.h"
#include "core/Vec2.h"

namespace si {

// Barreira destrutível bloco a bloco (estilo clássico). Blocos não-vivos
// deixam de existir: tiros atravessam o buraco.
class Shield {
public:
    static constexpr int kCols = static_cast<int>(cfg::kShieldWidth / cfg::kShieldBlockSize);
    static constexpr int kRows = static_cast<int>(cfg::kShieldWidth * 0.6f / cfg::kShieldBlockSize);
    static constexpr float kBlock = cfg::kShieldBlockSize;

    void spawn(Vec2 origin);

    [[nodiscard]] Vec2 origin() const { return origin_; }
    [[nodiscard]] const std::array<bool, static_cast<std::size_t>(kCols * kRows)>& blocks() const { return blocks_; }
    [[nodiscard]] bool blockAlive(int col, int row) const;

    [[nodiscard]] Rect blockRect(int col, int row) const;

    // Remove todo bloco sobreposto por `hit`; retorna true se algo quebrou.
    [[nodiscard]] bool destroy(Rect hit);

    [[nodiscard]] int blocksLeft() const;
    [[nodiscard]] bool empty() const;

private:
    Vec2 origin_{};
    std::array<bool, static_cast<std::size_t>(kCols * kRows)> blocks_{};
};

}  // namespace si