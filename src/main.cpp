#include <memory>
#include <string>

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

    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(kWindowWidth, kWindowHeight, "SPACE INVADERS");
    if (!IsWindowReady()) {
        si::logError("janela indisponivel");
        return 1;
    }
    SetWindowMinSize(si::cfg::kLogicalWidth / 2, si::cfg::kLogicalHeight / 2);
    SetTargetFPS(si::cfg::kTargetFps);
    // ESC é pausa dentro do jogo; sair é decisão do menu.
    SetExitKey(KEY_NULL);

    InitAudioDevice();

    si::Renderer renderer;
    if (!renderer.init(si::cfg::kLogicalWidth, si::cfg::kLogicalHeight)) {
        CloseAudioDevice();
        CloseWindow();
        return 1;
    }

    si::AudioManager audio;
    if (!audio.init()) {
        si::logWarn("seguindo sem audio");
    }

    si::HighScores highscores;
    const std::string hsPath = si::defaultHighScoresPath();
    highscores.load(hsPath);

    si::SceneManager scenes(si::cfg::kLogicalWidth, si::cfg::kLogicalHeight);

    si::AppContext app;
    app.audio = &audio;
    app.highscores = &highscores;
    app.scenes = &scenes;
    app.highScoresPath = hsPath;
    scenes.init(std::make_unique<si::MainMenuScene>(app));

    // O dt do frame vai cru para as cenas (entrada de UI é por frame); quem
    // precisa de determinismo — a simulação — acumula em passos fixos por
    // dentro (ver PlayingScene).
    while (!WindowShouldClose() && !app.quitRequested) {
        scenes.update(GetFrameTime());
        audio.updateMusic();

        renderer.begin();
        ClearBackground(si::toRay(si::palette().background));
        scenes.draw();
        renderer.end();
    }

    si::logInfo("encerrando");
    audio.shutdown();
    renderer.shutdown();
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
