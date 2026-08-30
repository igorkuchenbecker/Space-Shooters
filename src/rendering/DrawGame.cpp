#include "rendering/DrawGame.h"

#include <cmath>

#include "core/Config.h"
#include "rendering/Convert.h"

namespace si {

namespace {

constexpr float kAnimSpeed = 6.0f;

// Nave do jogador desenhada com triângulo + asas. `blink` >= 0 ativa o
// piscar de invulnerabilidade.
void drawPlayerShip(const Player& player, float time) {
    if (player.isInvulnerable() && std::fmod(time, 0.12f) < 0.06f) {
        return;  // pisca durante invuln
    }

    const ::Rectangle body = {player.pos.x, player.pos.y + 8.0f, player.w, player.h - 8.0f};
    DrawRectangleRec(body, toRay(palette().player));
    DrawRectangleRec({player.pos.x + 6.0f, player.pos.y + 2.0f, player.w - 12.0f, 8.0f}, toRay(palette().player));

    const float cx = player.pos.x + player.w * 0.5f;
    const float topY = player.pos.y + 2.0f;
    const float width = 7.0f + (player.moving ? 2.0f : 0.0f);
    DrawTriangle({cx, topY + 4.0f}, {cx - width, player.pos.y}, {cx + width, player.pos.y}, toRay(palette().player));

    // Fogo pulsante do motor.
    const float flame = std::sin(time * 18.0f) * 0.5f + 0.5f;
    DrawRectangleRec({cx - 2.0f, player.pos.y + player.h, 4.0f, 6.0f + flame * 5.0f},
                     {255, 180, 60, 230});
}

void drawProjectiles(const ProjectileManager& shots) {
    for (const auto& p : shots.shots()) {
        if (p.dead) {
            continue;
        }
        if (p.owner == Owner::Player) {
            const ::Rectangle r = {p.pos.x, p.pos.y, p.w, p.h};
            DrawRectangleRec(r, toRay(palette().playerShot));
            DrawRectangleRec({p.pos.x + p.w * 0.25f, p.pos.y - 3.0f, p.w * 0.5f, 3.0f},
                             toRay(palette().playerShot));
        } else {
            DrawRectangleRec({p.pos.x, p.pos.y, p.w, p.h * 0.6f}, toRay(palette().enemyShot));
            DrawRectangleRec({p.pos.x - 2.0f, p.pos.y + p.h * 0.6f, p.w + 4.0f, p.h * 0.2f},
                             toRay(palette().enemyShot));
            DrawRectangleRec({p.pos.x, p.pos.y + p.h * 0.8f, p.w, p.h * 0.2f}, toRay(palette().enemyShot));
        }
    }
}

void drawParticleSystem(const ParticleSystem& fx) {
    for (const auto& p : fx.particles()) {
        if (p.dead) {
            continue;
        }
        const float t = 1.0f - p.age / p.life;
        ::Color c = toRay(p.color);
        c.a = static_cast<unsigned char>(t * 255.0f);
        DrawRectangleRec({p.pos.x - p.size * 0.5f, p.pos.y - p.size * 0.5f, p.size, p.size}, c);
    }
}

}  // namespace

void drawEnemySprite(EnemyKind kind, Vec2 pos, float animPhase, ::Color tint) {
    // Dois quadros por sprite: as "pernas"/asas alternam com animPhase em [0,1).
    const bool frame = animPhase >= 0.5f;
    const float x = pos.x;
    const float y = pos.y;
    const float w = cfg::kEnemyWidth;
    const float h = cfg::kEnemyHeight;

    switch (kind) {
        case EnemyKind::Top: {  // aeronave larga
            DrawRectangleRec({x + w * 0.15f, y + 1.0f, w * 0.7f, h * 0.55f}, tint);
            DrawRectangleRec({x, y + h * 0.3f, w, h * 0.28f}, tint);
            if (frame) {
                DrawRectangleRec({x - 3.0f, y + h * 0.3f, 4.0f, h * 0.25f}, tint);
                DrawRectangleRec({x + w - 1.0f, y + h * 0.3f, 4.0f, h * 0.25f}, tint);
            } else {
                DrawRectangleRec({x - 3.0f, y + h * 0.5f, 4.0f, h * 0.2f}, tint);
                DrawRectangleRec({x + w - 1.0f, y + h * 0.5f, 4.0f, h * 0.2f}, tint);
            }
            DrawRectangleRec({x + w * 0.38f, y, w * 0.24f, h * 0.18f}, tint);  // antena
            break;
        }
        case EnemyKind::Mid: {  // "barata"
            DrawRectangleRec({x + w * 0.18f, y + h * 0.1f, w * 0.64f, h * 0.45f}, tint);
            DrawRectangleRec({x + w * 0.06f, y + h * 0.42f, w * 0.18f, h * 0.3f}, tint);
            DrawRectangleRec({x + w * 0.76f, y + h * 0.42f, w * 0.18f, h * 0.3f}, tint);
            DrawRectangleRec({x + w * 0.34f, y + h * 0.42f, w * 0.12f, h * 0.4f}, tint);
            DrawRectangleRec({x + w * 0.54f, y + h * 0.42f, w * 0.12f, h * 0.4f}, tint);
            break;
        }
        case EnemyKind::Low: {  // "caranguejo"
            DrawRectangleRec({x + w * 0.2f, y + 2.0f, w * 0.6f, h * 0.5f}, tint);
            DrawRectangleRec({x, y + 2.0f, w * 0.16f, h * 0.3f}, tint);
            DrawRectangleRec({x + w * 0.84f, y + 2.0f, w * 0.16f, h * 0.3f}, tint);
            if (frame) {
                DrawRectangleRec({x + w * 0.1f, y + h * 0.42f, w * 0.14f, h * 0.55f}, tint);
                DrawRectangleRec({x + w * 0.76f, y + h * 0.42f, w * 0.14f, h * 0.55f}, tint);
            } else {
                DrawRectangleRec({x + w * 0.26f, y + h * 0.5f, w * 0.14f, h * 0.5f}, tint);
                DrawRectangleRec({x + w * 0.6f, y + h * 0.5f, w * 0.14f, h * 0.5f}, tint);
            }
            break;
        }
    }
}

void drawSession(const GameSession& session, float time) {
    const float animPhase = std::fmod(time * kAnimSpeed, 1.0f);

    for (const auto& e : session.formation().enemies()) {
        if (e.alive) {
            drawEnemySprite(e.kind, e.pos, animPhase, enemyColorFor(e.kind));
        }
    }

    // Escudos antes da nave: a nave nunca pode ficar escondida atrás deles.
    const ::Color shieldC = toRay(palette().shield);
    for (const auto& shield : session.shields()) {
        for (int r = 0; r < Shield::kRows; ++r) {
            for (int c = 0; c < Shield::kCols; ++c) {
                if (shield.blockAlive(c, r)) {
                    DrawRectangleRec(toRay(shield.blockRect(c, r)), shieldC);
                }
            }
        }
    }

    drawPlayerShip(session.player(), time);

    drawProjectiles(session.projectiles());
    drawParticleSystem(session.particles());
}

}  // namespace si