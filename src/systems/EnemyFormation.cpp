#include "systems/EnemyFormation.h"

#include <algorithm>

#include "core/Config.h"

namespace si {

void EnemyFormation::reset(const FormationConfig& config) {
    cfg_ = config;
    enemies_.clear();
    enemies_.reserve(static_cast<std::size_t>(cfg_.cols) * static_cast<std::size_t>(cfg_.rows));

    const float gridWidth = static_cast<float>(cfg_.cols) * cfg_.spacingX;
    const float startX = (cfg_.leftBound + cfg_.rightBound) * 0.5f - gridWidth * 0.5f;

    for (int row = 0; row < cfg_.rows; ++row) {
        const EnemyKind kind = row <= 1 ? EnemyKind::Top : (row <= 3 ? EnemyKind::Mid : EnemyKind::Low);
        for (int col = 0; col < cfg_.cols; ++col) {
            Enemy e;
            e.kind = kind;
            e.scoreValue = scoreForKind(kind);
            e.pos = Vec2{startX + static_cast<float>(col) * cfg_.spacingX,
                         cfg_.startY + static_cast<float>(row) * cfg_.spacingY};
            enemies_.push_back(e);
        }
    }
    direction_ = 1;
    fireTimer_ = cfg_.baseSpeed > 0.0f ? 1.5f : 0.0f;
}

int EnemyFormation::aliveCount() const {
    return static_cast<int>(std::count_if(enemies_.begin(), enemies_.end(), [](const Enemy& e) { return e.alive; }));
}

int EnemyFormation::totalCount() const { return static_cast<int>(enemies_.size()); }

bool EnemyFormation::hasEnemies() const { return aliveCount() > 0; }

float EnemyFormation::minAliveX() const {
    float mn = cfg_.leftBound;
    bool found = false;
    for (const auto& e : enemies_) {
        if (e.alive && (!found || e.pos.x < mn)) {
            mn = e.pos.x;
            found = true;
        }
    }
    return found ? mn : cfg_.leftBound;
}

float EnemyFormation::maxAliveX() const {
    float mx = cfg_.leftBound;
    bool found = false;
    for (const auto& e : enemies_) {
        if (e.alive && (!found || e.pos.x + cfg::kEnemyWidth > mx)) {
            mx = e.pos.x + cfg::kEnemyWidth;
            found = true;
        }
    }
    return found ? mx : cfg_.leftBound;
}

float EnemyFormation::bottomY() const {
    float by = 0.0f;
    for (const auto& e : enemies_) {
        if (e.alive) {
            by = std::max(by, e.pos.y + cfg::kEnemyHeight);
        }
    }
    return by;
}

float EnemyFormation::effectiveSpeed() const {
    const int total = totalCount();
    if (total <= 0) {
        return cfg_.baseSpeed;
    }
    const float remaining = static_cast<float>(aliveCount()) / static_cast<float>(total);
    float tier = 1.0f;
    if (remaining <= 0.33f) {
        tier = 2.6f;
    } else if (remaining <= 0.66f) {
        tier = 1.6f;
    }
    return cfg_.baseSpeed * tier;
}

void EnemyFormation::shiftHorizontally(float delta) {
    for (auto& e : enemies_) {
        if (e.alive) {
            e.pos.x += delta;
        }
    }
}

void EnemyFormation::descend(float amount) {
    for (auto& e : enemies_) {
        if (e.alive) {
            e.pos.y += amount;
        }
    }
}

void EnemyFormation::update(float dt) {
    if (!hasEnemies()) {
        return;
    }

    const float step = effectiveSpeed() * dt * static_cast<float>(direction_);
    shiftHorizontally(step);

    const float mn = minAliveX();
    const float mx = maxAliveX();
    if (direction_ > 0 && mx > cfg_.rightBound) {
        shiftHorizontally(-(mx - cfg_.rightBound));
        direction_ = -1;
        descend(cfg_.descentStep);
    } else if (direction_ < 0 && mn < cfg_.leftBound) {
        shiftHorizontally(cfg_.leftBound - mn);
        direction_ = 1;
        descend(cfg_.descentStep);
    }
}

std::vector<Vec2> EnemyFormation::pollShots(Rng& rng, float fireInterval, float dt) {
    std::vector<Vec2> origins;
    if (!hasEnemies() || cfg_.cols <= 0) {
        return origins;
    }

    fireTimer_ -= dt;
    if (fireTimer_ > 0.0f) {
        return origins;
    }
    fireTimer_ = fireInterval > 0.0f ? fireInterval : 1.0f;

    // Atirador de cada coluna = o inimigo vivo mais baixo dela (padrão
    // clássico). A coluna vem do índice na grade, que nunca é reordenada.
    constexpr std::size_t kNone = static_cast<std::size_t>(-1);
    std::vector<std::size_t> bottomPerColumn(static_cast<std::size_t>(cfg_.cols), kNone);
    for (std::size_t i = 0; i < enemies_.size(); ++i) {
        const auto& e = enemies_[i];
        if (!e.alive) {
            continue;
        }
        const std::size_t col = i % static_cast<std::size_t>(cfg_.cols);
        const std::size_t current = bottomPerColumn[col];
        if (current == kNone || enemies_[current].pos.y < e.pos.y) {
            bottomPerColumn[col] = i;
        }
    }

    std::vector<std::size_t> candidates;
    candidates.reserve(bottomPerColumn.size());
    for (const std::size_t idx : bottomPerColumn) {
        if (idx != kNone) {
            candidates.push_back(idx);
        }
    }
    if (candidates.empty()) {
        return origins;
    }

    const int shooters = rng.intRange(1, std::min(kMaxShootersPerVolley, static_cast<int>(candidates.size())) + 1);
    origins.reserve(static_cast<std::size_t>(shooters));
    for (int i = 0; i < shooters && !candidates.empty(); ++i) {
        const std::size_t pick = static_cast<std::size_t>(rng.intRange(0, static_cast<int>(candidates.size())));
        const auto& e = enemies_[candidates[pick]];
        origins.push_back(Vec2{e.pos.x + cfg::kEnemyWidth * 0.5f, e.pos.y + cfg::kEnemyHeight});
        candidates.erase(candidates.begin() + static_cast<std::ptrdiff_t>(pick));
    }
    return origins;
}

void EnemyFormation::killAt(std::size_t index) {
    if (index < enemies_.size()) {
        enemies_[index].alive = false;
    }
}

void EnemyFormation::killAll() {
    for (auto& e : enemies_) {
        e.alive = false;
    }
}

}  // namespace si