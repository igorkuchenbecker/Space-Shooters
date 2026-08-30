#pragma once

#include <cstdint>

namespace si {

// Fatos de gameplay produzidos pela simulação pura. A camada de apresentação
// consome a fila e decide o que fazer (som, screen shake, telemetria) — assim
// o núcleo continua sem conhecer raylib e o áudio continua testável.
enum class GameEvent : std::uint8_t {
    PlayerShot,
    EnemyShot,
    EnemyKilled,
    ShieldChipped,
    PlayerHit,
    ExtraLife,
    LevelCleared,
    GameOver,
};

}  // namespace si
