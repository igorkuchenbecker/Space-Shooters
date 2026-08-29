#include "rendering/Renderer.h"

#include <algorithm>
#include <cmath>

#include "utils/Log.h"

namespace si {

Renderer::~Renderer() { shutdown(); }

bool Renderer::init(int logicalWidth, int logicalHeight) {
    target_ = LoadRenderTexture(logicalWidth, logicalHeight);
    if (target_.id == 0) {
        logError("falha ao criar render texture da tela virtual");
        return false;
    }
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

void Renderer::end() {
    EndTextureMode();

    const int winW = GetScreenWidth();
    const int winH = GetScreenHeight();
    const float scaleX = static_cast<float>(winW) / static_cast<float>(logicalWidth_);
    const float scaleY = static_cast<float>(winH) / static_cast<float>(logicalHeight_);
    const float fit = std::min(scaleX, scaleY);

    const float dw = static_cast<float>(logicalWidth_) * fit;
    const float dh = static_cast<float>(logicalHeight_) * fit;
    const float ox = (static_cast<float>(winW) - dw) * 0.5f;
    const float oy = (static_cast<float>(winH) - dh) * 0.5f;

    // source com altura negativa: a render texture é armazenada de ponta-cabeça.
    DrawTexturePro(target_.texture, {0.0f, 0.0f, static_cast<float>(logicalWidth_), -static_cast<float>(logicalHeight_)},
                   {ox, oy, dw, dh}, {0.0f, 0.0f}, 0.0f, WHITE);
}

}  // namespace si