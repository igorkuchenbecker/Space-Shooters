#pragma once

#include <cstdint>
#include <string>

#include "audio/AudioManager.h"
#include "HighScores.h"

namespace si {

class Scene;
class SceneManager;
class Renderer;

// Estado compartilhado entre cenas (não é uma "world", só o contexto do app).
struct AppContext {
    AudioManager* audio = nullptr;
    HighScores* highscores = nullptr;
    SceneManager* scenes = nullptr;
    std::string highScoresPath;

    std::int64_t lastScore = 0;
    int lastLevel = 0;
    bool lastWon = false;
    bool quitRequested = false;
};

}  // namespace si
