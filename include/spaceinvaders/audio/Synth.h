#pragma once

#include <cstdint>
#include <functional>
#include <vector>

namespace si::synth {

// Oscilador dado apenas o tempo; a frequência é capturada por quem monta.
using Osc = std::function<float(float t)>;

// Número de meio-tons a partir de A4 (positivo = mais agudo). Conveniência
// para compor melodias sem lembrar frequências.
float noteFreq(int semitonesFromA4);

// Osciladores básicos gerados para uma frequência fixa.
Osc sineOf(float freq);
Osc squareOf(float freq);
Osc sweep(float f0, float f1);  // varredura linear de frequência

struct Note {
    float frequency = 0.0f;  // 0 = pausa
    float duration = 0.25f;
};

// Renderiza PCM16 mono de um oscilador com envelope attack/release.
std::vector<std::int16_t> render(const Osc& osc, float durationSec, float gain, float attackSec, float releaseSec,
                                 unsigned sampleRate = 44100);

// Renderiza uma sequência melódica.
std::vector<std::int16_t> renderMelody(const std::vector<Note>& melody, float gain,
                                       unsigned sampleRate = 44100);

// Converte um buffer PCM16 em arquivo WAV em memória (para o raylib ler via
// LoadMusicStreamFromMemory/LoadSoundFromWave).
std::vector<unsigned char> toWavBytes(const std::vector<std::int16_t>& samples, unsigned sampleRate);

}  // namespace si::synth