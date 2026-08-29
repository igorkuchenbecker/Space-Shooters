#include "TestFramework.h"

#include "core/Color.h"
#include "core/FixedStep.h"
#include "core/LevelConfig.h"
#include "core/Rect.h"
#include "core/Rng.h"
#include "core/Vec2.h"

using namespace si;

namespace {

inline float vlen(Vec2 v) { return std::sqrtf(v.x*v.x + v.y*v.y); }

bool rangeContains(float v, float lo, float hi) { return v >= lo && v < hi; }

}  // namespace

TEST(ColorBasics) {
    const Color c = rgb(10, 20, 30);
    CHECK(c.r == 10 && c.g == 20 && c.b == 30 && c.a == 255);
    const Color half = rgba(1, 2, 3, 128);
    CHECK(half.a == 128);
}

TEST(Vec2Ops) {
    const Vec2 a{3.0f, 4.0f};
    CHECK_NEAR((a + Vec2{1.0f, 1.0f}).x, 4.0f, 1e-5f);
    CHECK_NEAR(std::sqrtf(a.x*a.x + a.y*a.y), 5.0f, 1e-4f);
}

TEST(RectOverlaps) {
    const Rect a = makeRect(Vec2{0.0f, 0.0f}, Vec2{10.0f, 10.0f});
    CHECK(overlaps(a, makeRect(Vec2{9.0f, 9.0f}, Vec2{10.0f, 10.0f})));
    CHECK(!overlaps(a, makeRect(Vec2{10.0f, 10.0f}, Vec2{10.0f, 10.0f})));
    const Rect b = makeRect(Vec2{5.0f, 5.0f}, Vec2{10.0f, 10.0f});
    CHECK(overlaps(a, b));
    CHECK(center(b).x == 10.0f);
}

TEST(RngDeterminism) {
    Rng a(42);
    Rng b(42);
    bool same = true;
    for (int i = 0; i < 1000; ++i) {
        if (a.next() != b.next()) {
            same = false;
            break;
        }
    }
    CHECK(same);

    Rng rng(1);
    for (int i = 0; i < 500; ++i) {
        const float v = rng.nextFloat();
        CHECK(v >= 0.0f && v < 1.0f);
        const int iv = rng.intRange(0, 5);
        CHECK(iv >= 0 && iv < 5);
        CHECK(rangeContains(rng.range(10.0f, 20.0f), 10.0f, 20.0f));
    }
    CHECK(!rng.chance(0.0f));
}

TEST(FixedStepAccumulates) {
    FixedStep step(1.0f / 60.0f);
    CHECK(step.push(0.001f) == 0);
    CHECK(step.push(0.032f) == 1);
    CHECK(step.push(0.0f) == 0);
    step.reset();
    CHECK(step.push(0.001f) == 0);
}

TEST(FixedStepCapsSpiral) {
    FixedStep step(1.0f / 60.0f);
    // dt gigante em um único frame: o acumulador é clampado a 0.25s, então
    // um push cede no máximo 15 passos (0.25 / (1/60)) — sem espiral da morte.
    const int steps = step.push(2.0f);
    CHECK(steps == 15);
    CHECK(step.push(1.0f / 60.0f) == 1);
}

TEST(LevelConfigBaseline) {
    const LevelConfig lvl1 = makeLevelConfig(1);
    CHECK(lvl1.formationSpeed == cfg::kFormationBaseSpeed);
    CHECK(lvl1.enemyShotSpeed == cfg::kEnemyShotBaseSpeed);
    CHECK(lvl1.enemyFireInterval == cfg::kEnemyFireBaseInterval);
    CHECK_NEAR(lvl1.formationSpeedMultiplier(), 1.0f, 1e-5f);
}

TEST(LevelConfigProgressionAndCaps) {
    const LevelConfig lvl4 = makeLevelConfig(4);
    CHECK(lvl4.formationSpeed > cfg::kFormationBaseSpeed);
    CHECK(lvl4.enemyShotSpeed > cfg::kEnemyShotBaseSpeed);
    CHECK(lvl4.enemyFireInterval < cfg::kEnemyFireBaseInterval);

    // Tetos aplicados, não crescem para sempre.
    const LevelConfig lvl100 = makeLevelConfig(100);
    const float cap = cfg::kFormationBaseSpeed * cfg::kLevelSpeedCap;
    CHECK(lvl100.formationSpeed <= cap * 1.0001f);
    CHECK(lvl100.enemyShotSpeed <= cfg::kLevelShotCap * 1.0001f);
    CHECK(lvl100.enemyFireInterval >= cfg::kLevelFireFloor * 0.9999f);
    CHECK(lvl100.formationDescent <= 44.0f);

    // Nível inválido vira nível 1.
    CHECK(makeLevelConfig(0).level == 1);
    CHECK(makeLevelConfig(-3).level == 1);
}