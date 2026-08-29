# Space Invaders

Um jogo arcade 2D em C++20 + raylib, inspirado no clássico _Space Invaders_.
Construído como "produto": sem relógio (timestep fixo de 60 Hz), tela virtual
960×540 com letterbox, áudio e música 100% sintetizados em memória (nenhum
arquivo de asset), high score persistido em arquivo e suporte a gamepad.

## Requisitos

- Compilador C++20 (GCC/Clang)
- CMake >= 3.16 e um gerador (ninja ou make)
- raylib >= 6.0

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

## Controles

| Ação          | Teclado                     | Gamepad                        |
|---------------|-----------------------------|--------------------------------|
| Mover         | → ← ou A / D                | direcional / analógico esquerdo |
| Atirar        | Espaço                      | botão A                        |
| Subir/descer menu | ↑ ↓ ou W / S            | direcional                      |
| Confirmar     | Enter / Espaço              | botão A                        |
| Voltar/Pausar | Esc                         | botão B                        |

## Pontuação

- Inimigo comum: 10 pontos por nível, 150 pontos no nível 4 (`Mystery`).
- Acertos dão 1 ponto extra por tiro que não atingiu nada.
- `Mystery` anda pelo topo; abata-o dentro do tempo na tela.
- High score (top 5) fica salvo em `spaceinvaders-highscores.txt` ao lado
  do executável.

## Estrutura

Veja `docs/architecture.md`. Resumo:

- `si_core` (zero raylib): toda a lógica de gameplay — inimigos, jogador,
  tiros, shields, partículas, pontuação, timestep fixo e spawn de inimigos.
- `spaceinvaders` (raylib): renderer com letterbox, cenas, áudio sintetizado
  e high scores.
- `si_tests`: framework de teste mínimo próprio, 33 casos / ~1600 verificações.

## Licença

MIT. Veja `LICENSE`.