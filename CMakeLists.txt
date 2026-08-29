cmake_minimum_required(VERSION 3.16)

project(spaceinvaders
        VERSION 1.0.0
        DESCRIPTION "Space Invaders arcade em C++20 + raylib"
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(SPACEINVADERS_WERROR "Trata warnings como erros" OFF)

# ----------------------------------------------------------------------------
# raylib: tenta target importado do config; senão cai no pkg-config.
# ----------------------------------------------------------------------------
find_package(raylib CONFIG QUIET)
if (NOT raylib_FOUND)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(RAYLIB_PC REQUIRED IMPORTED_TARGET raylib)
    set(RAYLIB_TARGET PkgConfig::RAYLIB_PC)
    set(RAYLIB_INCLUDE_DIRS "${RAYLIB_PC_INCLUDE_DIRS}")
else ()
    set(RAYLIB_TARGET raylib)
endif ()

# ----------------------------------------------------------------------------
# Núcleo puro: lógica de gameplay sem nenhuma dependência de raylib.
# ----------------------------------------------------------------------------
add_library(si_core STATIC
    src/core/FixedStep.cpp
    src/core/Rng.cpp
    src/core/GameSession.cpp
    src/entities/Enemy.cpp
    src/entities/Player.cpp
    src/entities/Shield.cpp
    src/entities/Particle.cpp
    src/systems/EnemyFormation.cpp
    src/systems/ProjectileManager.cpp
)

target_include_directories(si_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/spaceinvaders)
target_compile_features(si_core PUBLIC cxx_std_20)

# ----------------------------------------------------------------------------
# Executável do jogo.
# ----------------------------------------------------------------------------
add_executable(spaceinvaders
    src/main.cpp
    src/HighScores.cpp
    src/audio/AudioManager.cpp
    src/audio/Synth.cpp
    src/rendering/Renderer.cpp
    src/rendering/DrawGame.cpp
    src/rendering/Starfield.cpp
    src/rendering/Hud.cpp
    src/scenes/SceneManager.cpp
    src/scenes/MainMenuScene.cpp
    src/scenes/ControlsScene.cpp
    src/scenes/HighScoreScene.cpp
    src/scenes/PauseScene.cpp
    src/scenes/GameOverScene.cpp
    src/scenes/PlayingScene.cpp
)

target_link_libraries(spaceinvaders PRIVATE si_core ${RAYLIB_TARGET})
if (RAYLIB_INCLUDE_DIRS)
    target_include_directories(spaceinvaders PRIVATE ${RAYLIB_INCLUDE_DIRS})
endif ()

# ----------------------------------------------------------------------------
# Testes: framework mínimo próprio, linka apenas o núcleo.
# ----------------------------------------------------------------------------
enable_testing()
add_executable(si_tests
    tests/main.cpp
    tests/TestFramework.h
    tests/test_core.cpp
    tests/test_entities.cpp
    tests/test_systems.cpp
    tests/test_session.cpp
)
target_link_libraries(si_tests PRIVATE si_core)
add_test(NAME si_tests COMMAND si_tests)

# ----------------------------------------------------------------------------
# Warnings rigorosos (e opção de torná-los erro).
# ----------------------------------------------------------------------------
foreach (tgt si_core spaceinvaders si_tests)
    if (TARGET ${tgt})
        target_compile_options(${tgt} PRIVATE
            $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
            $<$<AND:$<CXX_COMPILER_ID:GNU,Clang>,$<BOOL:${SPACEINVADERS_WERROR}>>:-Werror>
        )
    endif ()
endforeach ()