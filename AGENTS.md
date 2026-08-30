# AGENTS.md

Guia de trabalho neste repositório (Space Invaders, C++20 + raylib).

## Convenções

- C++20, RAII, sem `new`/`delete` crus. Números mágicos justificados com
  `constexpr`.
- `si_core` = lógica pura, **sem raylib** (apenas `std::` e tipos `si::`).
  `spaceinvaders` = apresentação (window, render, áudio, cenas, persistência).
- O núcleo fala com a apresentação por `GameEvent` (fila drenada por frame),
  nunca por callback nem por chamada direta de áudio/render.
- Nomes em inglês, mensagens de log curtas; código sem comentário decorativo.
- Warnings `-Wall -Wextra -Wpedantic` obrigatórios; build com
  `-DSPACEINVADERS_WERROR=ON` para manter zero warnings.

## Build e testes

- `cmake -B build -G Ninja -DSPACEINVADERS_WERROR=ON && cmake --build build`
- `./build/si_tests` (deve fechar em 0 falhas)
- `ctest --test-dir build --output-on-failure`
- Sem raylib na máquina: `-DSPACEINVADERS_BUILD_GAME=OFF` compila e testa só
  o núcleo puro (é assim que a CI headless roda).
- Jogo: `./build/spaceinvaders` (janela 1280×720, vídeo lógico 960×540).

## Áudio (cuidado)

Samples são gerados em memória (`Synth`) e copiados para buffers alocados com
`RL_MALLOC`; a raylib assume a posse. **Nunca** passe `std::vector` para o
`Wave` nem chame `UnloadWave`/`unload` nesses buffers — resulta em
double-free. Ajuste de timbre: `include/spaceinvaders/audio/Synth.h`.

## Render (cuidado)

`Renderer::end()` é quem chama `BeginDrawing`/`EndDrawing`. Sem `EndDrawing`
não há troca de buffers **nem coleta de input**: a janela abre preta e
congelada mesmo com o resto do jogo funcionando. Não desenhe fora do par
`Renderer::begin()`/`Renderer::end()`.

## Timestep

A simulação roda em passos fixos dentro da `PlayingScene`, não no laço de
frames: as cenas recebem o dt cru (o input de menu é por frame e rodar a
cena N vezes duplicaria seleções). Passe sempre `cfg::kFixedDt` para
`GameSession::update`.

## Colisões (cuidado)

`GameSession::resolveCollisions` percorre os projéteis por índice. Nada pode
remover elementos do vetor no meio do laço — use `ProjectileManager::kill` /
`killAllOf` (que só marcam) e deixe o `sweepDead()` do fim varrer. Existe
teste de regressão para isso (`SessionPlayerHitKeepsPlayerShotAlive`).

## DoD

A fatia está pronta quando: build limpo com `-Werror`, testes verdes, e o
jogo abre sem crash por pelo menos 10 s (`timeout 10 ./build/spaceinvaders`)
**desenhando** de fato (uma captura da janela não pode ser toda preta).