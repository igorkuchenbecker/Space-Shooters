#include "audio/AudioManager.h"

#include <algorithm>
#include <cstring>
#include <cmath>

#include "audio/Synth.h"

#include <cstdio>

#include "utils/Log.h"

namespace si {

namespace {

constexpr float kMusicVolume = 0.5f;

// Cria um Sound raylib a partir de samples PCM16. O buffer é alocado com
// RL_MALLOC (a mesma família usada pela própria raylib) e copiado ali; em
// seguida a raylib assume a posse (ou copia) — nos dois casos é seguro
// nunca liberar manualmente ao longo de uma partida inteira.
Sound makeSound(const std::vector<std::int16_t>& samples) {
    const std::size_t n = samples.size();
    if (n == 0) {
        return Sound{};
    }
    void* data = RL_MALLOC(n * sizeof(std::int16_t));
    if (data == nullptr) {
        return Sound{};
    }
    std::memcpy(data, samples.data(), n * sizeof(std::int16_t));

    Wave wave = {};
    wave.frameCount = static_cast<unsigned>(n);
    wave.sampleRate = 44100;
    wave.sampleSize = 16;
    wave.channels = 1;
    wave.data = data;

    return LoadSoundFromWave(wave);
}

}  // namespace

bool AudioManager::init() {
    if (!IsAudioDeviceReady()) {
        logError("dispositivo de áudio indisponível — jogo sem som");
        return false;
    }
    if (!buildAllSounds()) {
        logError("falha ao sintetizar efeitos sonoros");
        return false;
    }
    hasMusic_ = buildMusic();
    if (!hasMusic_) {
        logWarn("falha ao sintetizar música — seguindo só com SFX");
    }
    setMasterVolume(masterVolume_);
    ready_ = true;
    logInfo("áudio sintetizado e pronto");
    return true;
}

void AudioManager::shutdown() {
    ready_ = false;
    hasMusic_ = false;
    if (music_.frameCount != 0) {
        UnloadMusicStream(music_);
        music_ = {};
    }
    musicWav_.clear();
    for (auto& s : sounds_) {
        if (s.frameCount != 0) {
            UnloadSound(s);
            s = {};
        }
    }
}

bool AudioManager::buildAllSounds() {
    using synth::sineOf;
    using synth::squareOf;
    using synth::sweep;

    // 1. Tiro do jogador: varredura rápida descendente.
    sounds_[static_cast<std::size_t>(Sfx::Shot)] = makeSound(synth::render(sweep(1000.0f, 250.0f), 0.10f, 0.25f, 0.002f, 0.05f));
    // 2. Tiro inimigo: varredura curta grave.
    sounds_[static_cast<std::size_t>(Sfx::EnemyShot)] = makeSound(synth::render(sweep(420.0f, 180.0f), 0.14f, 0.15f, 0.003f, 0.07f));
    // 3. Explosão: ruído com decaimento.
    sounds_[static_cast<std::size_t>(Sfx::Explosion)] = makeSound(
        synth::render([](float t) { return std::sin(t * 700.0f) * (1.0f - t) + std::sin(t * 1300.0f) * std::pow(1.0f - t, 2.0f); },
                      0.42f, 0.5f, 0.002f, 0.3f));
    // 4. Nave atingida: som grave + ruído.
    sounds_[static_cast<std::size_t>(Sfx::PlayerHit)] = makeSound(synth::render(sweep(300.0f, 60.0f), 0.34f, 0.55f, 0.004f, 0.22f));
    // 5. Bloco de barreira: clique agudo.
    sounds_[static_cast<std::size_t>(Sfx::ShieldHit)] = makeSound(synth::render(squareOf(1300.0f), 0.05f, 0.16f, 0.002f, 0.03f));
    // 6. Nível limpo: arpejo ascendente simples.
    sounds_[static_cast<std::size_t>(Sfx::LevelClear)] = makeSound(synth::renderMelody(
        {{synth::noteFreq(0), 0.09f}, {synth::noteFreq(4), 0.09f}, {synth::noteFreq(7), 0.09f}, {synth::noteFreq(12), 0.18f}},
        0.30f));
    // 7. Game over: arpejo menor descendente.
    sounds_[static_cast<std::size_t>(Sfx::GameOver)] = makeSound(synth::renderMelody(
        {{synth::noteFreq(0), 0.14f}, {synth::noteFreq(-3), 0.14f}, {synth::noteFreq(-7), 0.14f}, {synth::noteFreq(-12), 0.30f}},
        0.32f));
    // 8. Seleção de menu: blip curto.
    sounds_[static_cast<std::size_t>(Sfx::UiSelect)] = makeSound(synth::render(squareOf(880.0f), 0.06f, 0.20f, 0.002f, 0.04f));

    for (std::size_t i = 0; i < static_cast<std::size_t>(Sfx::Count); ++i) {
        if (!IsSoundValid(sounds_[i])) {
            char msg[64];
            std::snprintf(msg, sizeof msg, "sfx invalido no indice %zu", i);
            logError(msg);
            return false;
        }
    }
    return true;
}

bool AudioManager::buildMusic() {
    // A-minor arpejo simples e repetitivo (feel arcade), 8 compassos de 4.
    const std::vector<synth::Note> pattern = {
        {synth::noteFreq(-48), 0.5f}, {synth::noteFreq(-48), 0.5f}, {synth::noteFreq(-36), 0.5f}, {synth::noteFreq(-48), 0.5f},
        {synth::noteFreq(-31), 0.25f}, {synth::noteFreq(-36), 0.25f}, {synth::noteFreq(-41), 0.5f},
        {synth::noteFreq(-43), 0.5f}, {synth::noteFreq(-41), 0.5f}, {synth::noteFreq(-36), 1.0f},
    };
    std::vector<synth::Note> song;
    for (int loop = 0; loop < 2; ++loop) {
        song.insert(song.end(), pattern.begin(), pattern.end());
    }
    const auto samples = synth::renderMelody(song, 0.30f);
    musicWav_ = synth::toWavBytes(samples, 44100);
    if (musicWav_.empty()) {
        return false;
    }
    music_ = LoadMusicStreamFromMemory(".wav", musicWav_.data(), static_cast<int>(musicWav_.size()));
    music_.looping = true;
    return music_.frameCount != 0;
}

// Todas as chamadas são no-op quando a síntese falhou: o jogo roda mudo em
// vez de tocar em structs zeradas.
void AudioManager::play(Sfx sfx) {
    if (!ready_ || sfx >= Sfx::Count) {
        return;
    }
    PlaySound(sounds_[static_cast<std::size_t>(sfx)]);
}

void AudioManager::playMusic() {
    if (!hasMusic_) {
        return;
    }
    SetMusicVolume(music_, kMusicVolume * masterVolume_);
    PlayMusicStream(music_);
}

void AudioManager::pauseMusic() {
    if (hasMusic_) {
        PauseMusicStream(music_);
    }
}

void AudioManager::resumeMusic() {
    if (hasMusic_) {
        ResumeMusicStream(music_);
    }
}

void AudioManager::stopMusic() {
    if (hasMusic_) {
        StopMusicStream(music_);
    }
}

void AudioManager::updateMusic() {
    if (hasMusic_) {
        UpdateMusicStream(music_);
    }
}

void AudioManager::setMasterVolume(float volume) {
    masterVolume_ = std::clamp(volume, 0.0f, 1.0f);
    SetMasterVolume(masterVolume_);
}

bool AudioManager::isMusicPlaying() const { return hasMusic_ && IsMusicStreamPlaying(music_); }

}  // namespace si