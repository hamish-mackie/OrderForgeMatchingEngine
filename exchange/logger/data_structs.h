#pragma once

#include <fmt/core.h>

struct TestStructOriginal {
    uint64_t item1;
    uint32_t item2;
    uint32_t item3;
};

struct TestStructLog {
    void write(const TestStructOriginal& tso) {
        item1 = tso.item1;
        item2 = tso.item2;
        item3 = tso.item3;
    }
    uint64_t item1;
    uint32_t item2;
    uint32_t item3;

    std::string get_str() {
        return fmt::format("item1: {}, item2: {}, item3: {}", item1, item2, item3);
    }
};

