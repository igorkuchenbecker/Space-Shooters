#pragma once

#include "raylib.h"

namespace si {

// Tela virtual: renderiza tudo numa RenderTexture2D de resolução lógica fixa
// e blita escalada (com letterbox) para a janela, mantendo a proporção.
class Renderer {
public:
    ~Renderer();

    bool init(int logicalWidth, int logicalHeight);
    void shutdown();

    void begin();  // BeginTextureMode da resolução lógica
    void end();    // EndTextureMode + blit escalado com letterbox

    [[nodiscard]] int width() const { return logicalWidth_; }
    [[nodiscard]] int height() const { return logicalHeight_; }

private:
    RenderTexture2D target_{};
    int logicalWidth_ = 0;
    int logicalHeight_ = 0;
};

}  // namespace si