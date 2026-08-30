#pragma once

namespace si::cfg {

// Tela lógica (renderizada via RenderTexture2D e escalada para a janela).
inline constexpr int kLogicalWidth = 960;
inline constexpr int kLogicalHeight = 540;
inline constexpr int kTargetFps = 60;
inline constexpr float kFixedDt = 1.0f / 60.0f;

// Jogador.
inline constexpr float kPlayerWidth = 40.0f;
inline constexpr float kPlayerHeight = 24.0f;
inline constexpr float kPlayerSpeed = 340.0f;
inline constexpr float kPlayerFireCooldown = 0.34f;
inline constexpr float kPlayerInvulnTime = 2.0f;
inline constexpr int kStartLives = 3;
inline constexpr float kPlayerLineY = 496.0f;  // linha-base da nave (abaixo dos escudos)

// Formação de inimigos.
inline constexpr int kFormationCols = 11;
inline constexpr int kFormationRows = 5;
inline constexpr float kEnemyWidth = 34.0f;
inline constexpr float kEnemyHeight = 26.0f;
inline constexpr float kFormationSpacingX = 52.0f;
inline constexpr float kFormationSpacingY = 44.0f;
inline constexpr float kFormationStartY = 92.0f;
inline constexpr float kBoundaryMargin = 6.0f;
inline constexpr float kFormationBaseSpeed = 26.0f;
inline constexpr float kFormationDescentStep = 20.0f;
inline constexpr float kEnemyFireBaseInterval = 1.05f;

// Projéteis.
inline constexpr float kPlayerShotSpeed = 620.0f;
inline constexpr float kPlayerShotWidth = 4.0f;
inline constexpr float kPlayerShotHeight = 14.0f;
inline constexpr float kEnemyShotBaseSpeed = 210.0f;
inline constexpr float kEnemyShotWidth = 8.0f;
inline constexpr float kEnemyShotHeight = 16.0f;
inline constexpr float kEnemyShotMaxLifetime = 7.0f;

// Escudos (barreiras destrutíveis por bloco).
inline constexpr int kShieldCount = 4;
// Base dos escudos: 430 + 6 blocos de 8px = 478, logo acima da nave (496).
inline constexpr float kShieldY = 430.0f;
inline constexpr float kShieldWidth = 10.0f * 8.0f;  // 10 colunas de 8px
inline constexpr float kShieldBlockSize = 8.0f;

// Pontuação por tipo de inimigo.
inline constexpr int kScoreTopRow = 300;
inline constexpr int kScoreMidRow = 200;
inline constexpr int kScoreLowRow = 100;
inline constexpr int kScoreBonusLevelClear = 50;
// Vida extra a cada N pontos (recompensa clássica de fliperama).
inline constexpr int kExtraLifeEvery = 3000;
inline constexpr int kMaxLives = 6;

// Curvas de dificuldade por nível.
inline constexpr float kLevelSpeedFactor = 0.12f;
inline constexpr float kLevelSpeedCap = 2.0f;
inline constexpr float kLevelShotFactor = 18.0f;
inline constexpr float kLevelShotCap = 380.0f;
inline constexpr float kLevelFireFactor = 0.07f;
inline constexpr float kLevelFireFloor = 0.28f;

}  // namespace si::cfg