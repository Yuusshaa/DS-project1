// test/testkit.h -- pass/fail counters live in a struct, not in globals.
#pragma once
#include <cstdio>

struct Tally {
    int passed;
    int failed;
    unsigned int seed;     // for each test's own random number generator

    Tally() {
        passed = 0;
        failed = 0;
        seed = 12345;
    }
};

// Every test function takes a parameter named T of type Tally&.
#define CHECK(cond) \
    do { \
        if (cond) { T.passed = T.passed + 1; } \
        else { T.failed = T.failed + 1; std::printf("  FAIL line %d: %s\n", __LINE__, #cond); } \
    } while (0)

// Own random number generator (no library). Uses the seed stored in T.
inline int nextRandom(Tally& T) {
    T.seed = T.seed * 1103515245u + 12345u;
    return (int)((T.seed >> 16) & 0x7fff);
}