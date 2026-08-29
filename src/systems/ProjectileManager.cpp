#include "systems/ProjectileManager.h"

#include <algorithm>

namespace si {

void ProjectileManager::clear() { shots_.clear(); }

void ProjectileManager::addShot(Owner owner, Vec2 pos, Vec2 vel, float w, float h, float lifetime) {
    shots_.push_back(Projectile{owner, pos, vel, w, h, lifetime, false});
}

void ProjectileManager::update(float dt, float worldHeight) {
    for (auto& s : shots_) {
        if (s.dead) {
            continue;
        }
        s.pos += s.vel * dt;
        if (s.owner == Owner::Enemy) {
            s.life -= dt;
            if (s.life <= 0.0f) {
                s.dead = true;
            }
        }
        if (s.pos.y < -40.0f || s.pos.y > worldHeight + 40.0f) {
            s.dead = true;
        }
    }
    sweepDead();
}

void ProjectileManager::kill(std::size_t index) {
    if (index < shots_.size()) {
        shots_[index].dead = true;
    }
}

void ProjectileManager::sweepDead() {
    shots_.erase(std::remove_if(shots_.begin(), shots_.end(), [](const Projectile& s) { return s.dead; }),
                 shots_.end());
}

const std::vector<Projectile>& ProjectileManager::shots() const { return shots_; }

bool ProjectileManager::hasPlayerShot() const {
    for (const auto& s : shots_) {
        if (s.owner == Owner::Player) {
            return true;
        }
    }
    return false;
}

}  // namespace si