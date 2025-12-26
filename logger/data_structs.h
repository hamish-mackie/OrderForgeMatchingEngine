#pragma once

#include <fmt/core.h>

// wraps a format string to pass to the logger
struct FormatString {
    std::string str;
};

struct FormatStringLog {
    FormatStringLog() : str(256, ' ') {}

    void write(FormatString& debug) { str = debug.str; }
    std::string str;

    std::string get_str() {
        str.erase(std::find_if(str.rbegin(), str.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(),
                  str.end());
        return str;
    };
};

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

    std::string get_str() { return fmt::format("item1: {}, item2: {}, item3: {}", item1, item2, item3); }
};
