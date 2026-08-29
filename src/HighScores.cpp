#include "HighScores.h"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <fstream>

#include "raylib.h"

#include "utils/Log.h"

namespace si {

void HighScores::load(const std::string& path) {
    scores_.clear();
    std::ifstream in(path);
    if (!in) {
        return;  // primeira execução: sem histórico ainda
    }
    std::string line;
    while (std::getline(in, line)) {
        char* end = nullptr;
        const long long v = std::strtoll(line.c_str(), &end, 10);
        if (end != line.c_str() && v > 0) {
            add(static_cast<std::int64_t>(v));
        }
    }
}

void HighScores::save(const std::string& path) const {
    std::ofstream out(path);
    if (!out) {
        logWarn("não foi possível salvar high scores");
        return;
    }
    for (const auto s : scores_) {
        out << s << '\n';
    }
}

bool HighScores::qualifies(std::int64_t score) const {
    if (score <= 0) {
        return false;
    }
    if (scores_.size() < kTop) {
        return true;
    }
    return score > scores_.back();
}

void HighScores::add(std::int64_t score) {
    scores_.push_back(score);
    std::sort(scores_.begin(), scores_.end(), std::greater<std::int64_t>());
    if (scores_.size() > kTop) {
        scores_.resize(kTop);
    }
}

std::string defaultHighScoresPath() {
    return std::string(GetApplicationDirectory()) + "spaceinvaders-highscores.txt";
}

}  // namespace si