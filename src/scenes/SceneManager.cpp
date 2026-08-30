#include "scenes/SceneManager.h"

#include "raylib.h"

#include "rendering/Convert.h"
#include <algorithm>

#include "utils/Log.h"

namespace si {

SceneManager::SceneManager(int logicalWidth, int logicalHeight, float fadeDuration)
    : fadeDuration_(fadeDuration), logicalWidth_(logicalWidth), logicalHeight_(logicalHeight) {}

void SceneManager::init(std::unique_ptr<Scene> first) {
    stack_.clear();
    if (first) {
        first->onEnter();
        stack_.push_back(std::move(first));
    }
    transition_.reset();
}

void SceneManager::switchTo(std::unique_ptr<Scene> next) {
    if (!next) {
        return;
    }
    transition_ = Transition{std::move(next), 0.0f, 0};
}

void SceneManager::push(std::unique_ptr<Scene> next) {
    if (!next) {
        return;
    }
    logInfoWith("push cena", next->name());
    next->onEnter();
    stack_.push_back(std::move(next));
}

void SceneManager::pop() {
    if (stack_.empty()) {
        return;
    }
    logInfoWith("pop cena", stack_.back()->name());
    stack_.back()->onExit();
    stack_.pop_back();
}

const Scene* SceneManager::top() const { return stack_.empty() ? nullptr : stack_.back().get(); }

void SceneManager::commitSwitch() {
    logInfoWith("troca de cena", transition_->next->name());
    for (auto& s : stack_) {
        s->onExit();
    }
    stack_.clear();
    transition_->next->onEnter();
    stack_.push_back(std::move(transition_->next));
    transition_->phase = 1;
    transition_->elapsed = 0.0f;
}

void SceneManager::advance(float dt) {
    auto& t = *transition_;
    t.elapsed += dt;
    if (t.phase == 0 && t.elapsed >= fadeDuration_) {
        commitSwitch();
        return;
    }
    if (t.phase == 1) {
        if (!stack_.empty()) {
            stack_.back()->update(dt);  // a nova cena já reage durante o fade-in
        }
        if (t.elapsed >= fadeDuration_) {
            transition_.reset();
        }
    }
}

void SceneManager::update(float dt) {
    if (transition_) {
        advance(dt);
        return;
    }
    if (!stack_.empty()) {
        stack_.back()->update(dt);
    }
}

void SceneManager::draw() {
    for (auto& s : stack_) {
        s->draw();
    }

    if (transition_) {
        float alpha = 0.0f;
        if (transition_->phase == 0) {
            alpha = transition_->elapsed / fadeDuration_;
        } else {
            alpha = 1.0f - transition_->elapsed / fadeDuration_;
        }
        alpha = std::clamp(alpha, 0.0f, 1.0f);
        DrawRectangle(0, 0, logicalWidth_, logicalHeight_,
                      {0, 0, 0, static_cast<unsigned char>(alpha * 255.0f)});
    }
}

}  // namespace si