#pragma once
#include <fstream>
#include <fmt/core.h>

#include <string_view>

class Logger {
public:
    static Logger& get_instance() {
        static Logger instance;
        return instance;
    }

    template<typename ... Args>
    void log(std::string_view log_prepend, std::string_view format_str, Args&&... args) {
        log_file_ << fmt::format("{} {}\n", log_prepend, fmt::vformat(format_str, fmt::make_format_args(args...)));
    }

    Logger(const Logger&) = delete;
    Logger(Logger&&) = delete;

    Logger& operator=(const Logger&) = delete;
    Logger& operator=(Logger&&) = delete;

private:
    std::ofstream log_file_;

    Logger() {
        log_file_.open("log.txt", std::ios::out | std::ios::ate);
        if(!log_file_.is_open()) { fmt::print("could not open log file"); }
    }
};

template <std::size_t...Idxs>
constexpr auto substring_as_array(std::string_view str, std::index_sequence<Idxs...>)
{
    return std::array{str[Idxs]...};
}

constexpr auto compile_time_string_merge(std::string_view s1, std::string_view s2, std::string_view s3) {
    return fmt::format(FMT_STRING("[{}] [{}::{}]"), s1 ,s2, s3);
}

template <typename T>
constexpr auto type_name_array()
{
#if defined(__clang__)
    constexpr auto prefix   = std::string_view{"[T = "};
    constexpr auto suffix   = std::string_view{"]"};
    constexpr auto function = std::string_view{__PRETTY_FUNCTION__};
#elif defined(__GNUC__)
    constexpr auto prefix   = std::string_view{"with T = "};
    // constexpr auto suffix   = std::string_view{"&]"};
    constexpr auto function = std::string_view{__PRETTY_FUNCTION__};
#elif defined(_MSC_VER)
    constexpr auto prefix   = std::string_view{"type_name_array<"};
    constexpr auto suffix   = std::string_view{">(void)"};
    constexpr auto function = std::string_view{__FUNCSIG__};
#else
# error Unsupported compiler
#endif

    constexpr auto start = function.find(prefix) + prefix.size();
    constexpr auto end = function.find_first_of("(<&[", start);

    static_assert(start < end);

    constexpr auto name = function.substr(start, (end - start));
    return substring_as_array(name, std::make_index_sequence<name.size()>{});
}

template <typename T>
struct type_name_holder {
    static inline constexpr auto value = type_name_array<T>();
};

template <typename T>
constexpr auto type_name() -> std::string_view
{
    constexpr auto& value = type_name_holder<T>::value;
    return std::string_view{value.data(), value.size()};
}
// #define DISABLE_LOGGING

#ifdef DISABLE_LOGGING
#define LOG_INFO(format_str, ...)
#define LOG_WARN(format_str, ...)
#define LOG_ERROR(format_str, ...)
#else
#define LOG_INFO(format_str, ...) Logger::get_instance().log(compile_time_string_merge("INFO", type_name<decltype(*this)>(), std::string_view(__FUNCTION__)), format_str, ##__VA_ARGS__)
#define LOG_WARN(format_str, ...) Logger::get_instance().log(compile_time_string_merge("WARN", type_name<decltype(*this)>(), std::string_view(__FUNCTION__)), format_str, ##__VA_ARGS__)
#define LOG_ERROR(format_str, ...) Logger::get_instance().log(compile_time_string_merge("ERROR", type_name<decltype(*this)>(), std::string_view(__FUNCTION__)), format_str, ##__VA_ARGS__)

#endif
