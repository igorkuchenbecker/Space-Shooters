#pragma once

#include "raylib.h"

#include "core/GameSession.h"

namespace si {

// Desenha um estado de GameSession inteiro em primitivas raylib. `time` é o
// relógio global (segundos), usado para animações de sprite/flicker.
void drawSession(const GameSession& session, float time);

void drawEnemySprite(EnemyKind kind, Vec2 pos, float animPhase, ::Color tint);

}  // namespace si