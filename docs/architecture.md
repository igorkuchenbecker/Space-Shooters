# Arquitetura

O jogo é dividido em duas camadas rígidas:

```
si_core        lógica pura, determinística, 100% testável (zero raylib)
spaceinvaders  apresentação raylib (window, render, áudio, cenas, persistência)
```

- A camada raylib **não** contém regras de gameplay: ela lê estado, desenha e
  encaminha input.
- A camada pura **não** conhece raylib: usa apenas `std::` e estruturas
  próprias (`si::Vec2`, `si::Rect`, `si::Color`).

## Lógica pura (`si_core`)

- `core/` — `Vec2`, `Rect`, `Color`, `Config` (tamanhos/velocidades),
  `LevelConfig` (formação por nível), `FixedStep` (timestep fixo 60 Hz com
  `push` clampado a 0,25 s), `Rng` (xorshift determinístico com seed),
  `GameSession` (orquestra a partida: jogador, formação, tiros, shields,
  partículas, pontuação, vidas e status Playing/Won/Lost), `GameEvent`
  (fila de fatos de gameplay consumida pela apresentação).
- `entities/` — `Enemy`, `Player`, `Projectile`, `Shield` (máscara de pixels),
  `Particle`.
- `systems/` — `EnemyFormation` (grade, movimento de queda, disparos,
  colisões), `ProjectileManager` (tiros do jogador e inimigos, life e sweep).

Cada módulo tem teste dedicado em `tests/`. O `GameSession` concentra as
regras de pontuação e transições de estado; `EnemyFormation` é testado com
cenários de colisão e fim de linha.

## Apresentação raylib (`spaceinvaders`)

- `main.cpp` — inicialização (janela 1280×720 redimensionável, áudio), laço
  de frames, delega ao `SceneManager`. `SetExitKey(KEY_NULL)` impede que o
  Esc de pausa feche a janela.
- `rendering/` — `Renderer` (tela virtual 960×540 renderizada em
  `RenderTexture` e apresentada dentro de `BeginDrawing`/`EndDrawing`, com
  letterbox), `Convert` (paleta `si::Color` ↔ raylib),
  `DrawGame` (desenha sessão: formação, shields, tiros, partículas),
  `Starfield` (parallax de fundo), `Hud` (score, high score, vidas, nível).
- `scenes/` — `Scene` (interface), `SceneManager` (pilha e fade entre cenas),
  cenas: `MainMenu`, `Controls`, `HighScore`, `Pause`, `GameOver`, `Playing`.
  `UiInput` unifica teclado e gamepad na navegação de menus.
- `audio/` — `Synth` (gera PCM16: sweep, onda quadrada, ruído, melodia),
  `AudioManager` (8 SFX + música em memória). Os buffers são alocados com
  `RL_MALLOC` e a raylib assume a posse (nunca passa por um `std::vector`
  temporário).
- `HighScores` — top 5 persistido em `spaceinvaders-highscores.txt`.

## Decisões principais

- **Timestep fixo**: `FixedStep` acumula dt e executa até 15 passos de 1/60 s
  por `push`. Quem o usa é a `PlayingScene`, não o laço de frames: a entrada
  de UI é por frame (`IsKeyPressed` é um evento de frame, e repetir a cena
  N vezes duplicaria cliques de menu), enquanto a simulação roda em passos
  fixos por dentro. `GameSession::update` recebe sempre `cfg::kFixedDt`, o
  que torna o gameplay determinístico e igual em 30, 60 ou 144 FPS.

- **Eventos em vez de callbacks**: a simulação empurra `GameEvent`
  (`PlayerShot`, `EnemyKilled`, `PlayerHit`, `ExtraLife`, …) numa fila; a
  `PlayingScene` drena a fila e traduz para SFX. O núcleo continua sem saber
  o que é som, e o mapeamento evento→som é testável isoladamente.
- **Redesigned draw**: o jogo desenha na `RenderTexture` 960×540 e a escala
  para a janela preservando a proporção (letterbox preto).
- **Áudio sem assets**: todo áudio é síntese em memória — o binário não
  depende de arquivos externos em runtime.
- **Shields destrutíveis**: tiros do jogador e dos inimigos corroem a
  barreira bloco a bloco; partículas marcam a lasca. Os escudos ficam acima
  da linha da nave (nunca a escondem) e são desenhados antes dela.

## Testes

`tests/TestFramework.h` define o micro-framework (registro por seção e
`CHECK`). `si_tests` roda todos os módulos e reporta a contagem ao final (0 falhas).
`si_core` e `si_tests` não linkam raylib, então a suíte roda em CI headless
com `-DSPACEINVADERS_BUILD_GAME=OFF`.