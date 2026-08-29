# AGENTS.md

Guia de trabalho neste repositório (Space Invaders, C++20 + raylib).

## Convenções

- C++20, RAII, sem `new`/`delete` crus. Números mágicos justificados com
  `constexpr`.
- `si_core` = lógica pura, **sem raylib** (apenas `std::` e tipos `si::`).
  `spaceinvaders` = apresentação (window, render, áudio, cenas, persistência).
- Nomes em inglês, mensagens de log curtas; código sem comentário decorativo.
- Warnings `-Wall -Wextra -Wpedantic` obrigatórios; build com
  `-DSPACEINVADERS_WERROR=ON` para manter zero warnings.

## Build e testes

- `cmake -B build -G Ninja -DSPACEINVADERS_WERROR=ON && cmake --build build`
- `./build/si_tests` (33 casos, ~1600 verificações — deve fechar em 0 falhas)
- `ctest --test-dir build --output-on-failure`
- Jogo: `./build/spaceinvaders` (janela 1280×720, vídeo lógico 960×540).

## Áudio (cuidado)

Samples são gerados em memória (`Synth`) e copiados para buffers alocados com
`RL_MALLOC`; a raylib assume a posse. **Nunca** passe `std::vector` para o
`Wave` nem chame `UnloadWave`/`unload` nesses buffers — resulta em
double-free. Ajuste de timbre: `include/spaceinvaders/audio/Synth.h`.

## DoD

A fatia está pronta quando: build limpo com `-Werror`, testes verdes, e o
jogo abre sem crash por pelo menos 10 s (`timeout 10 ./build/spaceinvaders`).