#include "TestFramework.h"

#include <cstdlib>
#include <cstring>

int main() {
    const auto& cases = test::registry();
    std::printf("rodando %zu casos de teste\n", cases.size());
    for (const auto& c : cases) {
        const int before = test::failures();
        c.fn();
        const bool ok = test::failures() == before;
        std::printf("  [%s] %s\n", ok ? "OK" : "FAIL", c.name.c_str());
    }
    std::printf("%d verificações, %d falhas\n", test::checks(), test::failures());
    return test::failures() == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}