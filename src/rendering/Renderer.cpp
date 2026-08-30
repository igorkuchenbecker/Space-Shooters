#include "rendering/Renderer.h"

#include <algorithm>

#include "rendering/Convert.h"
#include "utils/Log.h"

namespace si {

Renderer::~Renderer() { shutdown(); }

bool Renderer::init(int logicalWidth, int logicalHeight) {
    target_ = LoadRenderTexture(logicalWidth, logicalHeight);
    if (target_.id == 0) {
        logError("falha ao criar render texture da tela virtual");
        return false;
    }
    // A tela virtual é pixel-art: sem filtragem, sem sangramento nas bordas.
    SetTextureFilter(target_.texture, TEXTURE_FILTER_POINT);
    SetTextureWrap(target_.texture, TEXTURE_WRAP_CLAMP);
    logicalWidth_ = logicalWidth;
    logicalHeight_ = logicalHeight;
    logInfo("renderer da tela virtual inicializado");
    return true;
}

void Renderer::shutdown() {
    if (target_.id != 0) {
        UnloadRenderTexture(target_);
        target_ = {};
    }
}

void Renderer::begin() { BeginTextureMode(target_); }

Rect Renderer::viewport() const {
    const float winW = static_cast<float>(GetScreenWidth());
    const float winH = static_cast<float>(GetScreenHeight());
    const float fit = std::min(winW / static_cast<float>(logicalWidth_), winH / static_cast<float>(logicalHeight_));

    const float dw = static_cast<float>(logicalWidth_) * fit;
    const float dh = static_cast<float>(logicalHeight_) * fit;
    return Rect{(winW - dw) * 0.5f, (winH - dh) * 0.5f, dw, dh};
}

void Renderer::end() {
    EndTextureMode();

    const Rect dest = viewport();

    // BeginDrawing/EndDrawing são obrigatórios: é o EndDrawing que troca os
    // buffers e coleta os eventos de input do frame.
    BeginDrawing();
    ClearBackground(::Color{0, 0, 0, 255});  // barras do letterbox
    // source com altura negativa: a render texture é armazenada de ponta-cabeça.
    DrawTexturePro(target_.texture,
                   {0.0f, 0.0f, static_cast<float>(logicalWidth_), -static_cast<float>(logicalHeight_)},
                   toRay(dest), {0.0f, 0.0f}, 0.0f, ::Color{255, 255, 255, 255});
    EndDrawing();
}

}  // namespace si
