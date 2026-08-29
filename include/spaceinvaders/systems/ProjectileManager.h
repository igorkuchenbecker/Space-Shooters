#pragma once

#include <vector>

#include "entities/Projectile.h"

namespace si {

class ProjectileManager {
public:
    void clear();

    void addShot(Owner owner, Vec2 pos, Vec2 vel, float w, float h, float lifetime);

    void update(float dt, float worldHeight);

    void kill(std::size_t index);

    void sweepDead();

    [[nodiscard]] const std::vector<Projectile>& shots() const;

    [[nodiscard]] bool hasPlayerShot() const;

private:
    std::vector<Projectile> shots_;
};

}  // namespace si