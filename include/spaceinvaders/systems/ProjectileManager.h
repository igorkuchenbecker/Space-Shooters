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

    // Marca todos os tiros de um dono como mortos (sem varrer o vetor, para
    // não invalidar índices de quem estiver iterando).
    void killAllOf(Owner owner);

    void sweepDead();

    [[nodiscard]] const std::vector<Projectile>& shots() const;

    [[nodiscard]] bool hasPlayerShot() const;

    [[nodiscard]] std::size_t count() const { return shots_.size(); }

private:
    std::vector<Projectile> shots_;
};

}  // namespace si