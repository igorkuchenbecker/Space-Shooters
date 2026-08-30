#pragma once

#include <cstdio>
#include <string_view>

namespace si {

// Log mínimo e não-invasivo: só eventos de ciclo de vida/erro, nada de ruído
// por frame. Prefixo único facilita grep no terminal. Usa stderr (não sofre
// bufferização quando o stdout é desviado para arquivo/pipe).
inline void logInfo(const char* msg) { std::fprintf(stderr, "[SPACEINVADERS] %s\n", msg); }
inline void logWarn(const char* msg) { std::fprintf(stderr, "[SPACEINVADERS][WARN] %s\n", msg); }
inline void logError(const char* msg) { std::fprintf(stderr, "[SPACEINVADERS][ERROR] %s\n", msg); }

// Evento de ciclo de vida com um alvo nomeado (ex: "push cena", "Pause").
inline void logInfoWith(const char* msg, std::string_view target) {
    std::fprintf(stderr, "[SPACEINVADERS] %s: %.*s\n", msg, static_cast<int>(target.size()), target.data());
}

}  // namespace si