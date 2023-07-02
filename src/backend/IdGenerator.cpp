#include <random>

#include "IdGenerator.h"

template<size_t N>
constexpr static size_t length(char const (&)[N]) {
    return N - 1;
}

constexpr static char symbols[] =
        "qwertyuiopasdfghjklzxcvbnm"
        "QWERTYUIOPASDFGHJKLZXCVBNM"
        "1234567890";

static std::random_device rd;
static std::mt19937 rng {rd()};
static std::uniform_int_distribution<int> uid(0, length(symbols) - 1);

QString IdGenerator::generate(int len) {
    QString result;

    for (int i = 0; i < len; ++i)
        result += symbols[uid(rng)];

    return result;
}
