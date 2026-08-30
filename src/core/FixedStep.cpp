#include "core/FixedStep.h"

#include <cmath>

namespace si {

FixedStep::FixedStep(float stepSeconds) : step_(stepSeconds) {}

int FixedStep::push(float frameDt) {
    // Um passo não-positivo (ou NaN) tornaria o laço abaixo infinito.
    if (!(step_ > 0.0f)) {
        return 0;
    }
    if (!std::isfinite(frameDt) || frameDt <= 0.0f) {
        return 0;
    }

    accumulator_ += frameDt < kMaxFrameDt ? frameDt : kMaxFrameDt;
    int steps = 0;
    while (accumulator_ >= step_) {
        accumulator_ -= step_;
        ++steps;
    }
    return steps;
}

void FixedStep::reset() { accumulator_ = 0.0f; }

float FixedStep::alpha() const { return step_ > 0.0f ? accumulator_ / step_ : 0.0f; }

}  // namespace si
