#pragma once

#include "raylib.h"

namespace si {

// Navegação de UI consistente: teclado + gamepad mapeados para as mesmas
// ações. Facilita testar e manter o mesmo feel em todo menu.
namespace ui {

inline bool upPressed() {
    return IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W) ||
           (IsGamepadAvailable(0) && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_UP));
}

inline bool downPressed() {
    return IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S) ||
           (IsGamepadAvailable(0) && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_DOWN));
}

inline bool confirmPressed() {
    return IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE) ||
           (IsGamepadAvailable(0) && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN));
}

inline bool backPressed() {
    return IsKeyPressed(KEY_ESCAPE) ||
           (IsGamepadAvailable(0) && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT));
}

}  // namespace ui

}  // namespace si