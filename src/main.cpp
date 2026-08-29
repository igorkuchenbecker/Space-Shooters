#include <algorithm>
#include <cstdint>
#include <memory>

#include "raylib.h"

#include "audio/AudioManager.h"
#include "core/Config.h"
#include "HighScores.h"
#include "rendering/Convert.h"
#include "rendering/Renderer.h"
#include "scenes/Context.h"
#include "scenes/MainMenuScene.h"
#include "scenes/SceneManager.h"
#include "utils/Log.h"

namespace {

constexpr int kWindowWidth = 1280;
constexpr int kWindowHeight = 720;

}  // namespace

int main() {
    si::logInfo("space invaders iniciando");

    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(kWindowWidth, kWindowHeight, "SPACE INVADERS");
    SetTargetFPS(si::cfg::kTargetFps);
    InitAudioDevice();
    if (!IsAudioDeviceReady()) {
        si::logError("dispositivo de áudio falhou ao inicializar");
    }

    if (IsWindowReady()) {
        si::logInfo("janela pronta");
    } else {
        return 1;
    }

    si::Renderer renderer;
    if (!renderer.init(si::cfg::kLogicalWidth, si::cfg::kLogicalHeight)) {
        CloseWindow();
        return 1;
    }

    si::AudioManager audio;
    audio.init();

    si::HighScores highscores;
    const std::string hsPath = si::defaultHighScoresPath();
    highscores.load(hsPath);

    si::SceneManager scenes(si::cfg::kLogicalWidth, si::cfg::kLogicalHeight);

    si::AppContext app;
    app.audio = &audio;
    app.highscores = &highscores;
    app.scenes = &scenes;
    scenes.init(std::make_unique<si::MainMenuScene>(app));

    while (!WindowShouldClose() && !app.quitRequested) {
        const float dt = std::min(GetFrameTime(), 0.25f);

        scenes.update(dt);
        audio.updateMusic();

        renderer.begin();
        ClearBackground(si::toRay(si::palette().background));
        scenes.draw();
        renderer.end();
    }

    // Se o jogador sair no meio de uma partida, o placar global não muda —
    // pontuações só valem registradas pelo game over. Nada a fazer aqui além
    // de garantir flush de um game-over que não tenha persistido (defensivo).

    si::logInfo("encerrando");
    audio.shutdown();
    renderer.shutdown();
    CloseAudioDevice();
    CloseWindow();
    return 0;
}