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
  partículas, pontuação, vidas, fases Intro/Playing/Cleared/GameOver).
- `entities/` — `Enemy`, `Player`, `Projectile`, `Shield` (máscara de pixels),
  `Particle`.
- `systems/` — `EnemyFormation` (grade, movimento de queda, disparos,
  colisões), `ProjectileManager` (tiros do jogador e inimigos, life e sweep).

Cada módulo tem teste dedicado em `tests/`. O `GameSession` concentra as
regras de pontuação e transições de estado; `EnemyFormation` é testado com
cenários de colisão e fim de linha.

## Apresentação raylib (`spaceinvaders`)

- `main.cpp` — inicialização (janela 1280×720, audio), loop de frames com
  timestep fixo, delega ao `SceneManager`.
- `rendering/` — `Renderer` (tela virtual 960×540 renderizada em
  `RenderTexture` com letterbox), `Convert` (paleta `si::Color` ↔ raylib),
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
  por `push`; `GameSession::update` roda com dt fixo, determinístico.
- **Redesigned draw**: o jogo desenha na `RenderTexture` 960×540 e a escala
  para a janela preservando a proporção (letterbox preto).
- **Áudio sem assets**: todo áudio é síntese em memória — o binário não
  depende de arquivos externos em runtime.
- **Shields orgânicos**: `Mystery` e inimigos danificam shields; partículas
  cinzas mostram a lasca no pixel.

## Testes

`tests/TestFramework.h` define o micro-framework (registro por seção e
`CHECK`). `si_tests` roda todos os módulos; a contagem de verificações é
reportada ao final (esperado: 33 casos, ~1600 verificações, 0 falhas).