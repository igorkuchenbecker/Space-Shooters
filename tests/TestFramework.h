#pragma once

// Framework de teste mínimo, estilo único TU: registrar + executar no main.
// Sem dependências externas — o núcleo não conhece raylib nem GTest.

#include <cmath>
#include <cstdio>
#include <functional>
#include <string>
#include <vector>

namespace test {

struct Case {
    std::string name;
    std::function<void()> fn;
};

inline std::vector<Case>& registry() {
    static std::vector<Case> cases;
    return cases;
}

inline int& failures() {
    static int count = 0;
    return count;
}

inline int& checks() {
    static int count = 0;
    return count;
}

inline void fail(const char* file, int line, const std::string& msg) {
    ++failures();
    std::fprintf(stderr, "  FAIL %s:%d  %s\n", file, line, msg.c_str());
}

inline bool near(double a, double b, double eps) { return std::fabs(a - b) <= eps; }

struct Registrar {
    Registrar(const char* name, std::function<void()> fn) { registry().push_back({name, std::move(fn)}); }
};

}  // namespace test

#define TEST_CONCAT_IMPL(a, b) a##b
#define TEST_CONCAT(a, b) TEST_CONCAT_IMPL(a, b)

#define TEST(name)                                                                                 \
    static void name();                                                                            \
    static ::test::Registrar TEST_CONCAT(reg_, __LINE__)(#name, name);                             \
    static void name()

#define CHECK(expr)                                                                                \
    do {                                                                                           \
        ++::test::checks();                                                                        \
        if (!(expr)) {                                                                             \
            ::test::fail(__FILE__, __LINE__, "CHECK: " #expr);                                     \
        }                                                                                          \
    } while (0)

#define CHECK_EQ(a, b)                                                                             \
    do {                                                                                           \
        ++::test::checks();                                                                        \
        const auto va = (a);                                                                       \
        const auto vb = (b);                                                                       \
        if (!(va == vb)) {                                                                         \
            ::test::fail(__FILE__, __LINE__, "CHECK_EQ " #a " == " #b);                            \
        }                                                                                          \
    } while (0)

#define CHECK_NEAR(a, b, eps)                                                                      \
    do {                                                                                           \
        ++::test::checks();                                                                        \
        const auto va = (a);                                                                       \
        const auto vb = (b);                                                                       \
        if (!::test::near(static_cast<double>(va), static_cast<double>(vb), (eps))) {              \
            ::test::fail(__FILE__, __LINE__, "CHECK_NEAR " #a " ~= " #b);                          \
        }                                                                                          \
    } while (0)


