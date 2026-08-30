# Space Invaders

Um jogo arcade 2D em C++20 + raylib, inspirado no clássico _Space Invaders_.
Construído como "produto": simulação em timestep fixo de 60 Hz (independente
do FPS da tela), tela virtual 960×540 com letterbox, áudio e música 100%
sintetizados em memória (nenhum arquivo de asset), high score persistido em
arquivo e suporte a gamepad.

## Requisitos

- Compilador C++20 (GCC/Clang)
- CMake >= 3.16 e um gerador (ninja ou make)
- raylib >= 5.5 (apenas para o executável do jogo)

## Build e execução

```sh
cmake -B build -G Ninja -DSPACEINVADERS_WERROR=ON
cmake --build build
./build/spaceinvaders
```

Testes da lógica pura:

```sh
ctest --test-dir build --output-on-failure
```

`si_core` e `si_tests` não dependem de raylib. Em CI headless (ou em qualquer
máquina sem a biblioteca) dá para compilar e testar só o núcleo:

```sh
cmake -B build -G Ninja -DSPACEINVADERS_WERROR=ON -DSPACEINVADERS_BUILD_GAME=OFF
cmake --build build && ctest --test-dir build --output-on-failure
```

## Controles

| Ação          | Teclado                     | Gamepad                         |
|---------------|-----------------------------|---------------------------------|
| Mover         | → ← ou A / D                | direcional / analógico esquerdo |
| Atirar        | Espaço                      | botão A                         |
| Subir/descer menu | ↑ ↓ ou W / S            | direcional                      |
| Confirmar     | Enter / Espaço              | botão A                         |
| Voltar/Pausar | Esc                         | botão B                         |

## Pontuação

- Fileiras superiores (`Top`): 300 pontos. Centrais (`Mid`): 200. Inferior
  (`Low`): 100.
- Limpar o nível dá `50 × nível` de bônus.
- Vida extra a cada 3000 pontos, até o teto de 6 vidas.
- A formação acelera conforme os inimigos morrem; cada nível aumenta
  velocidade, cadência e velocidade dos tiros inimigos, com tetos.
- High score (top 5) fica salvo em `spaceinvaders-highscores.txt` ao lado
  do executável.

## Estrutura

Veja `docs/architecture.md`. Resumo:

- `si_core` (zero raylib): toda a lógica de gameplay — inimigos, jogador,
  tiros, shields, partículas, pontuação, timestep fixo, spawn de inimigos e a
  fila de eventos de gameplay.
- `spaceinvaders` (raylib): renderer com letterbox, cenas, áudio sintetizado
  e high scores.
- `si_tests`: framework de teste mínimo próprio.

## Licença

MIT. Veja `LICENSE`.
