#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace si {

// Top 5 de pontuações persistido em arquivo texto (um número por linha).
class HighScores {
public:
    static constexpr std::size_t kTop = 5;

    void load(const std::string& path);
    void save(const std::string& path) const;

    [[nodiscard]] bool qualifies(std::int64_t score) const;
    void add(std::int64_t score);

    [[nodiscard]] const std::vector<std::int64_t>& list() const { return scores_; }
    [[nodiscard]] std::int64_t best() const {
        return scores_.empty() ? 0 : scores_.front();
    }

private:
    std::vector<std::int64_t> scores_;
};

// Caminho de dados próximo ao executável (portável entre SOs).
std::string defaultHighScoresPath();

}  // namespace si