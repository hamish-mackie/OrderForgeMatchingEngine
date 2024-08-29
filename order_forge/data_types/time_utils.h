#pragma once

#include <chrono>
namespace of {

inline clock_t get_nano_ts() {
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
}
} // namespace of
