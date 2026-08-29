#include "core/FixedStep.h"

namespace si {

FixedStep::FixedStep(float stepSeconds) : step_(stepSeconds) {}

int FixedStep::push(float frameDt) {
    // Clamp grande para evitar espiral da morte após hicks/breakpoints.
    const float maxDt = 0.25f;
    accumulator_ += frameDt < maxDt ? frameDt : maxDt;
    int steps = 0;
    while (accumulator_ >= step_) {
        accumulator_ -= step_;
        ++steps;
    }
    return steps;
}

void FixedStep::reset() { accumulator_ = 0.0f; }

}  // namespace si