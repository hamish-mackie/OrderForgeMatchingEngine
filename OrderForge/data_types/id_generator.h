#pragma once

constexpr uint64_t COUNTER_SPACE = 1000000000ULL;

// Id generator used to generate unique trade id's and order id's

// They need to be independent accross unique assets and times/days
// Will start by using the first 8 digits as year, month, day and then a counter.

template<typename T>
class IdGenerator {
public:
    static IdGenerator& instance() {
        static IdGenerator instance;
        return instance;
    }

    uint64_t generate_id() {
        return ++counter_;
    }
private:
    uint64_t counter_;

    IdGenerator(): counter_(get_current_datetime() * COUNTER_SPACE) {}

    static uint64_t get_current_datetime() {
        const auto now = std::chrono::system_clock::now();
        std::time_t now_c = std::chrono::system_clock::to_time_t(now);
        std::tm now_tm = *std::localtime(&now_c);

        const auto year = now_tm.tm_year + 1900;  // tm_year is years since 1900
        const auto month = now_tm.tm_mon + 1;     // tm_mon is months since January [0,11]
        const auto day = now_tm.tm_mday;          // tm_mday is day of the month [1,31]

        // Combine year, month, and day into a YYYYMMDD format
        return (year * 10000ULL) + (month * 100ULL) + day;
    }
};