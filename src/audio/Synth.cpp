#include "audio/Synth.h"

#include <algorithm>
#include <cmath>

namespace si::synth {

float noteFreq(int semitonesFromA4) {
    constexpr float kA4 = 440.0f;
    return kA4 * std::pow(2.0f, static_cast<float>(semitonesFromA4) / 12.0f);
}

Osc sineOf(float freq) { return [freq](float t) { return std::sin(2.0f * 3.14159265358979f * freq * t); }; }

Osc squareOf(float freq) { return [freq](float t) { return std::sin(2.0f * 3.14159265358979f * freq * t) >= 0.0f ? 1.0f : -1.0f; }; }

Osc sweep(float f0, float f1) {
    return [f0, f1](float t) {
        const float k = 2.0f * 3.14159265358979f;
        return std::sin(k * (f0 * t + 0.5f * (f1 - f0) * t * t));
    };
}

std::vector<std::int16_t> render(const Osc& osc, float durationSec, float gain, float attackSec, float releaseSec,
                                 unsigned sampleRate) {
    const std::size_t n = static_cast<std::size_t>(durationSec * static_cast<float>(sampleRate));
    std::vector<std::int16_t> out;
    out.reserve(n);

    const float attack = std::max(0.001f, attackSec);
    const float release = std::max(0.001f, releaseSec);

    for (std::size_t i = 0; i < n; ++i) {
        const float t = static_cast<float>(i) / static_cast<float>(sampleRate);
        float env = t < attack ? t / attack : 1.0f;
        const float tLeft = durationSec - t;
        if (t > durationSec - release) {
            env *= tLeft / release;
        }
        env = std::clamp(env, 0.0f, 1.0f);
        const float v = osc(t) * env * gain;
        out.push_back(static_cast<std::int16_t>(std::clamp(v, -1.0f, 1.0f) * 32767.0f));
    }
    return out;
}

std::vector<std::int16_t> renderMelody(const std::vector<Note>& melody, float gain,
                                       unsigned sampleRate) {
    std::vector<std::int16_t> out;
    for (const auto& note : melody) {
        if (note.frequency <= 0.0f) {
            out.insert(out.end(), static_cast<std::size_t>(note.duration * static_cast<float>(sampleRate)), 0);
            continue;
        }
        const std::size_t count = static_cast<std::size_t>(note.duration * static_cast<float>(sampleRate));
        const std::size_t start = out.size();
        out.resize(start + count);
        for (std::size_t i = 0; i < count; ++i) {
            const float t = static_cast<float>(i) / static_cast<float>(sampleRate);
            const float env = std::min(1.0f, t / 0.01f) * std::min(1.0f, (note.duration - t) / 0.06f);
            out[start + i] = static_cast<std::int16_t>(
                std::clamp(squareOf(note.frequency)(t) * env * gain, -1.0f, 1.0f) * 32767.0f);
        }
    }
    return out;
}

std::vector<unsigned char> toWavBytes(const std::vector<std::int16_t>& samples, unsigned sampleRate) {
    const unsigned dataSize = static_cast<unsigned>(samples.size()) * 2u;
    const unsigned byteRate = sampleRate * 2u;

    std::vector<unsigned char> wav;
    wav.reserve(44 + dataSize);

    auto push32 = [&wav](unsigned v) {
        wav.push_back(static_cast<unsigned char>(v & 0xFFu));
        wav.push_back(static_cast<unsigned char>((v >> 8) & 0xFFu));
        wav.push_back(static_cast<unsigned char>((v >> 16) & 0xFFu));
        wav.push_back(static_cast<unsigned char>((v >> 24) & 0xFFu));
    };
    auto push16 = [&wav](unsigned v) {
        wav.push_back(static_cast<unsigned char>(v & 0xFFu));
        wav.push_back(static_cast<unsigned char>((v >> 8) & 0xFFu));
    };

    wav.insert(wav.end(), {'R', 'I', 'F', 'F'});
    push32(36 + dataSize);
    wav.insert(wav.end(), {'W', 'A', 'V', 'E'});
    wav.insert(wav.end(), {'f', 'm', 't', ' '});
    push32(16);
    push16(1);  // PCM
    push16(1);  // mono
    push32(sampleRate);
    push32(byteRate);
    push16(2);   // block align
    push16(16);  // bits por amostra
    wav.insert(wav.end(), {'d', 'a', 't', 'a'});
    push32(dataSize);
    for (const auto s : samples) {
        push16(static_cast<unsigned>(static_cast<std::int16_t>(s)));
    }
    return wav;
}

}  // namespace si::synth