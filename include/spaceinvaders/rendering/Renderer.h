#pragma once

#include "raylib.h"

#include "core/Rect.h"

namespace si {

// Tela virtual: renderiza tudo numa RenderTexture2D de resolução lógica fixa
// e blita escalada (com letterbox) para a janela, mantendo a proporção.
class Renderer {
public:
    ~Renderer();

    Renderer() = default;
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    bool init(int logicalWidth, int logicalHeight);
    void shutdown();

    void begin();  // BeginTextureMode da resolução lógica
    void end();    // EndTextureMode + BeginDrawing/blit com letterbox/EndDrawing

    // Retângulo da janela ocupado pela tela lógica (o resto são as barras).
    [[nodiscard]] Rect viewport() const;

    [[nodiscard]] int width() const { return logicalWidth_; }
    [[nodiscard]] int height() const { return logicalHeight_; }

private:
    RenderTexture2D target_{};
    int logicalWidth_ = 0;
    int logicalHeight_ = 0;
};

}  // namespace si
