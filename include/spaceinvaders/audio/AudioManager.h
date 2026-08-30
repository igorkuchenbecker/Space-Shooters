#pragma once

#include <cstdint>
#include <vector>

#include "raylib.h"

namespace si {

enum class Sfx : std::uint8_t {
    Shot,
    EnemyShot,
    Explosion,
    PlayerHit,
    ShieldHit,
    LevelClear,
    GameOver,
    UiSelect,
    Count,
};

// Camada única de áudio. Todos os sons e a música são sintetizados em tempo
// de inicialização (sem arquivos de assets) e mantidos em memória durante a
// execução — nada é carregado por frame.
class AudioManager {
public:
    AudioManager() = default;
    ~AudioManager() { shutdown(); }
    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;

    bool init();
    void shutdown();

    void play(Sfx sfx);

    void playMusic();
    void pauseMusic();
    void resumeMusic();
    void stopMusic();
    void updateMusic();

    void setMasterVolume(float volume);

    [[nodiscard]] bool isMusicPlaying() const;
    [[nodiscard]] bool ready() const { return ready_; }

private:
    Sound sounds_[static_cast<std::size_t>(Sfx::Count)]{};
    Music music_{};
    std::vector<unsigned char> musicWav_;  // mantido vivo enquanto a música existe
    float masterVolume_ = 0.85f;
    bool ready_ = false;      // SFX sintetizados com sucesso
    bool hasMusic_ = false;   // música carregada (pode faltar mesmo com SFX ok)

    bool buildAllSounds();
    bool buildMusic();
};

}  // namespace si