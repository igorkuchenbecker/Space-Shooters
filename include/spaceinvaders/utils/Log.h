#pragma once

#include <cstdio>

namespace si {

// Log mínimo e não-invasivo: só eventos de ciclo de vida/erro, nada de ruído
// por frame. Prefixo único facilita grep no terminal. Usa stderr (não sofre
// bufferização quando o stdout é desviado para arquivo/pipe).
inline void logInfo(const char* msg) { std::fprintf(stderr, "[SPACEINVADERS] %s\n", msg); }
inline void logWarn(const char* msg) { std::fprintf(stderr, "[SPACEINVADERS][WARN] %s\n", msg); }
inline void logError(const char* msg) { std::fprintf(stderr, "[SPACEINVADERS][ERROR] %s\n", msg); }

}  // namespace si