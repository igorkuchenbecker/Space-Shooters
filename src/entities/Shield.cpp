#include "entities/Shield.h"

#include <array>
#include <cstddef>

namespace si {

namespace {

// Máscara clássica da barreira (10 colunas x 6 linhas): topo abaulado e
// arco recortado embaixo, onde a nave se abriga. '#' = bloco vivo.
constexpr const char* kMask[Shield::kRows] = {
    "..######..",
    ".########.",
    "##########",
    "##########",
    "###....###",
    "##......##",
};

constexpr std::size_t kIndex(int col, int row) { return static_cast<std::size_t>(row * Shield::kCols + col); }

}  // namespace

void Shield::spawn(Vec2 origin) {
    origin_ = origin;
    for (int r = 0; r < kRows; ++r) {
        for (int c = 0; c < kCols; ++c) {
            blocks_[kIndex(c, r)] = kMask[r][c] == '#';
        }
    }
}

bool Shield::blockAlive(int col, int row) const {
    if (col < 0 || col >= kCols || row < 0 || row >= kRows) {
        return false;
    }
    return blocks_[kIndex(col, row)];
}

Rect Shield::blockRect(int col, int row) const {
    return makeRect(origin_.x + static_cast<float>(col) * kBlock,
                    origin_.y + static_cast<float>(row) * kBlock, kBlock, kBlock);
}

bool Shield::destroy(Rect hit) {
    bool any = false;
    for (int r = 0; r < kRows; ++r) {
        for (int c = 0; c < kCols; ++c) {
            if (blocks_[kIndex(c, r)] && overlaps(hit, blockRect(c, r))) {
                blocks_[kIndex(c, r)] = false;
                any = true;
            }
        }
    }
    return any;
}

int Shield::blocksLeft() const {
    int count = 0;
    for (const bool b : blocks_) {
        count += b ? 1 : 0;
    }
    return count;
}

bool Shield::empty() const { return blocksLeft() == 0; }

}  // namespace si